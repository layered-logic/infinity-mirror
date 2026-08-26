/*
 * Multi-network Wi-Fi credential store — ESP-IDF side.
 *
 * See ll_wifi.h for the wiring order. Pure-C list operations live in
 * ll_wifi_logic.c so they can be unit-tested on the host. This file is
 * the NVS persistence layer plus the migration shim.
 */

#include "ll_wifi.h"
#include "ll_wifi_logic.h"

#include <stdio.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "nvs.h"
#include "nvs_flash.h"

static const char *TAG = "ll_wifi";

#define LL_WIFI_NS         "ll_wifi"
#define LL_WIFI_KEY_COUNT  "count"
#define LL_WIFI_KEY_ACTIVE "active"
/* Per-entry keys are "entry_0".."entry_3" — built with snprintf. */

static ll_wifi_list_t   g_list;
static SemaphoreHandle_t g_lock;
static bool             g_inited;

/* ---- low-level NVS helpers ------------------------------------------------ */

static void entry_key(uint8_t idx, char *out, size_t out_size)
{
    /* %hhu, not %u: idx is a uint8_t so the output is at most "entry_255"
     * (10 bytes incl. NUL) into a 16-byte buffer. The (unsigned) cast threw
     * that range away, and at -Os GCC inlines entry_key -> erase_entry ->
     * persist_locked, assumes the full 32-bit range, computes 17 bytes and
     * trips -Werror=format-truncation. No behaviour change. 2026-08-25. */
    snprintf(out, out_size, "entry_%hhu", idx);
}

static esp_err_t open_rw(nvs_handle_t *h)
{
    return nvs_open(LL_WIFI_NS, NVS_READWRITE, h);
}

static esp_err_t write_count(nvs_handle_t h, uint8_t count)
{
    return nvs_set_u8(h, LL_WIFI_KEY_COUNT, count);
}

static esp_err_t write_active(nvs_handle_t h, uint8_t active)
{
    return nvs_set_u8(h, LL_WIFI_KEY_ACTIVE, active);
}

static esp_err_t write_entry(nvs_handle_t h, uint8_t idx,
                             const ll_wifi_entry_t *entry)
{
    char key[16];
    entry_key(idx, key, sizeof(key));
    return nvs_set_blob(h, key, entry, sizeof(*entry));
}

static esp_err_t erase_entry(nvs_handle_t h, uint8_t idx)
{
    char key[16];
    entry_key(idx, key, sizeof(key));
    esp_err_t err = nvs_erase_key(h, key);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return ESP_OK;
    }
    return err;
}

/* Persist the in-memory list to NVS in full. Used after any mutation —
 * the per-entry key layout means we only rewrite the entries that
 * actually changed plus the count/active keys, but for simplicity we
 * just rewrite everything that's currently valid + erase the trailing
 * vacated slot if any. */
static esp_err_t persist_locked(uint8_t prev_count)
{
    nvs_handle_t h;
    esp_err_t err = open_rw(&h);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_open(RW): %s", esp_err_to_name(err));
        return err;
    }

    /* Write all currently-valid entries. */
    for (uint8_t i = 0; i < g_list.count; i++) {
        err = write_entry(h, i, &g_list.entries[i]);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "write entry_%u: %s", i, esp_err_to_name(err));
            nvs_close(h);
            return err;
        }
    }
    /* Erase any slots that used to be populated but aren't now (after a
     * remove). */
    for (uint8_t i = g_list.count; i < prev_count && i < LL_WIFI_LIST_MAX; i++) {
        err = erase_entry(h, i);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "erase entry_%u: %s", i, esp_err_to_name(err));
            /* Non-fatal — the count tells boot-time loaders to ignore this. */
        }
    }

    err = write_count(h, g_list.count);
    if (err == ESP_OK) {
        err = write_active(h, g_list.active_idx);
    }
    if (err == ESP_OK) {
        err = nvs_commit(h);
    }
    nvs_close(h);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "persist commit: %s", esp_err_to_name(err));
    }
    return err;
}

/* ---- init / load ---------------------------------------------------------- */

static void load_locked(void)
{
    ll_wifi_list_init(&g_list);

    nvs_handle_t h;
    esp_err_t err = nvs_open(LL_WIFI_NS, NVS_READONLY, &h);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "namespace empty (first boot)");
        return;
    }
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "nvs_open(RO): %s — starting empty",
                 esp_err_to_name(err));
        return;
    }

    uint8_t count = 0;
    err = nvs_get_u8(h, LL_WIFI_KEY_COUNT, &count);
    if (err != ESP_OK) {
        if (err != ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGW(TAG, "read count: %s", esp_err_to_name(err));
        }
        nvs_close(h);
        return;
    }
    if (count > LL_WIFI_LIST_MAX) {
        ESP_LOGW(TAG, "stored count=%u > %u — treating as corrupt, starting empty",
                 (unsigned)count, (unsigned)LL_WIFI_LIST_MAX);
        nvs_close(h);
        return;
    }

    uint8_t loaded = 0;
    for (uint8_t i = 0; i < count; i++) {
        char key[16];
        entry_key(i, key, sizeof(key));

        ll_wifi_entry_t entry;
        size_t size = sizeof(entry);
        err = nvs_get_blob(h, key, &entry, &size);
        if (err != ESP_OK || size != sizeof(entry)) {
            ESP_LOGW(TAG, "read %s: %s (size=%u) — stopping load at %u",
                     key, esp_err_to_name(err), (unsigned)size, (unsigned)i);
            break;
        }
        if (!ll_wifi_entry_sanitize(&entry)) {
            ESP_LOGW(TAG, "%s sanitize failed — stopping load at %u",
                     key, (unsigned)i);
            break;
        }
        g_list.entries[loaded] = entry;
        loaded++;
    }
    g_list.count = loaded;

    uint8_t active = LL_WIFI_ACTIVE_NONE;
    err = nvs_get_u8(h, LL_WIFI_KEY_ACTIVE, &active);
    if (err == ESP_OK && active < g_list.count) {
        g_list.active_idx = active;
    } else {
        g_list.active_idx = LL_WIFI_ACTIVE_NONE;
    }

    nvs_close(h);

    ESP_LOGI(TAG, "loaded count=%u active=%u",
             (unsigned)g_list.count, (unsigned)g_list.active_idx);
}

esp_err_t ll_wifi_init(void)
{
    if (g_inited) {
        return ESP_OK;
    }
    g_lock = xSemaphoreCreateMutex();
    if (!g_lock) {
        return ESP_ERR_NO_MEM;
    }
    if (xSemaphoreTake(g_lock, portMAX_DELAY) == pdTRUE) {
        load_locked();
        xSemaphoreGive(g_lock);
    }
    g_inited = true;
    return ESP_OK;
}

/* ---- migration shim (§5.3) ------------------------------------------------ */

esp_err_t ll_wifi_migrate_from_esp_wifi(const char *ssid, const char *password)
{
    if (!g_inited) {
        return ESP_ERR_INVALID_STATE;
    }
    if (ssid == NULL || ssid[0] == '\0') {
        return ESP_OK;  /* nothing to migrate */
    }
    if (password == NULL) {
        password = "";
    }

    esp_err_t err = ESP_OK;
    if (xSemaphoreTake(g_lock, portMAX_DELAY) != pdTRUE) {
        return ESP_FAIL;
    }
    if (g_list.count != 0) {
        /* Already populated — migration already happened or the user
         * has saved networks via the new flow. Don't disturb. */
        xSemaphoreGive(g_lock);
        return ESP_OK;
    }

    ll_wifi_add_result_t r = ll_wifi_list_add(&g_list, ssid, password);
    if (r != LL_WIFI_ADD_OK_INSERTED) {
        ESP_LOGW(TAG, "migration add failed: r=%d", (int)r);
        xSemaphoreGive(g_lock);
        return ESP_FAIL;
    }
    err = persist_locked(0);
    xSemaphoreGive(g_lock);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "migrated esp_wifi cred → ll_wifi[0] (ssid=\"%s\")",
                 ssid);
    }
    return err;
}

/* ---- public API ----------------------------------------------------------- */

void ll_wifi_get_list(ll_wifi_list_t *out)
{
    if (out == NULL) {
        return;
    }
    if (!g_inited) {
        ll_wifi_list_init(out);
        return;
    }
    if (xSemaphoreTake(g_lock, portMAX_DELAY) == pdTRUE) {
        *out = g_list;
        xSemaphoreGive(g_lock);
    } else {
        ll_wifi_list_init(out);
    }
}

uint8_t ll_wifi_count(void)
{
    if (!g_inited) {
        return 0;
    }
    uint8_t c = 0;
    if (xSemaphoreTake(g_lock, portMAX_DELAY) == pdTRUE) {
        c = g_list.count;
        xSemaphoreGive(g_lock);
    }
    return c;
}

bool ll_wifi_get_by_ssid(const char *ssid, ll_wifi_entry_t *out)
{
    if (!g_inited || ssid == NULL || out == NULL) {
        return false;
    }
    bool found = false;
    if (xSemaphoreTake(g_lock, portMAX_DELAY) == pdTRUE) {
        int idx = ll_wifi_list_find(&g_list, ssid);
        if (idx >= 0) {
            *out = g_list.entries[idx];
            found = true;
        }
        xSemaphoreGive(g_lock);
    }
    return found;
}

uint8_t ll_wifi_get_active_idx(void)
{
    if (!g_inited) {
        return LL_WIFI_ACTIVE_NONE;
    }
    uint8_t a = LL_WIFI_ACTIVE_NONE;
    if (xSemaphoreTake(g_lock, portMAX_DELAY) == pdTRUE) {
        a = g_list.active_idx;
        xSemaphoreGive(g_lock);
    }
    return a;
}

ll_wifi_add_result_t ll_wifi_add(const char *ssid, const char *password)
{
    if (!g_inited) {
        return LL_WIFI_ADD_ERR_INVALID;
    }
    ll_wifi_add_result_t r = LL_WIFI_ADD_ERR_INVALID;
    if (xSemaphoreTake(g_lock, portMAX_DELAY) == pdTRUE) {
        uint8_t prev_count = g_list.count;
        r = ll_wifi_list_add(&g_list, ssid, password);
        if (r == LL_WIFI_ADD_OK_INSERTED || r == LL_WIFI_ADD_OK_UPDATED) {
            esp_err_t err = persist_locked(prev_count);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "add persist failed: %s",
                         esp_err_to_name(err));
                /* In-memory state is already mutated; on next boot, NVS
                 * load will produce whatever survived the partial write.
                 * No graceful rollback for now. */
            }
        }
        xSemaphoreGive(g_lock);
    }
    return r;
}

ll_wifi_remove_result_t ll_wifi_remove(const char *ssid)
{
    if (!g_inited) {
        return LL_WIFI_REMOVE_NOT_FOUND;
    }
    ll_wifi_remove_result_t r = LL_WIFI_REMOVE_NOT_FOUND;
    if (xSemaphoreTake(g_lock, portMAX_DELAY) == pdTRUE) {
        uint8_t prev_count = g_list.count;
        r = ll_wifi_list_remove(&g_list, ssid);
        if (r == LL_WIFI_REMOVE_OK) {
            esp_err_t err = persist_locked(prev_count);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "remove persist failed: %s",
                         esp_err_to_name(err));
            }
        }
        xSemaphoreGive(g_lock);
    }
    return r;
}

bool ll_wifi_bump_priority(const char *ssid, int64_t last_used_us)
{
    if (!g_inited || ssid == NULL) {
        return false;
    }
    bool ok = false;
    if (xSemaphoreTake(g_lock, portMAX_DELAY) == pdTRUE) {
        int idx = ll_wifi_list_find(&g_list, ssid);
        if (idx >= 0) {
            g_list.entries[idx].last_used_us = last_used_us;
            esp_err_t err = persist_locked(g_list.count);
            if (err != ESP_OK) {
                ESP_LOGW(TAG, "bump_priority persist: %s", esp_err_to_name(err));
            }
            ok = true;
        }
        xSemaphoreGive(g_lock);
    }
    return ok;
}

esp_err_t ll_wifi_set_active(uint8_t idx, int64_t last_used_us)
{
    if (!g_inited) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t err = ESP_ERR_INVALID_ARG;
    if (xSemaphoreTake(g_lock, portMAX_DELAY) == pdTRUE) {
        if (idx < g_list.count && idx < LL_WIFI_LIST_MAX) {
            g_list.entries[idx].last_used_us = last_used_us;
            g_list.active_idx = idx;
            err = persist_locked(g_list.count);
        }
        xSemaphoreGive(g_lock);
    }
    return err;
}

void ll_wifi_clear_active(void)
{
    if (!g_inited) {
        return;
    }
    if (xSemaphoreTake(g_lock, portMAX_DELAY) == pdTRUE) {
        if (g_list.active_idx != LL_WIFI_ACTIVE_NONE) {
            g_list.active_idx = LL_WIFI_ACTIVE_NONE;
            (void)persist_locked(g_list.count);
        }
        xSemaphoreGive(g_lock);
    }
}

esp_err_t ll_wifi_erase_all(void)
{
    if (!g_inited) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t err = ESP_OK;
    if (xSemaphoreTake(g_lock, portMAX_DELAY) == pdTRUE) {
        ll_wifi_list_init(&g_list);

        nvs_handle_t h;
        err = open_rw(&h);
        if (err == ESP_OK) {
            err = nvs_erase_all(h);
            if (err == ESP_OK) {
                err = nvs_commit(h);
            }
            nvs_close(h);
        } else if (err == ESP_ERR_NVS_NOT_FOUND) {
            err = ESP_OK;  /* nothing to erase */
        }
        xSemaphoreGive(g_lock);
    }
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "namespace erased");
    }
    return err;
}
