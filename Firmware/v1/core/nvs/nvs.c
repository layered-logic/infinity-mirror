/*
 * NVS-backed settings persistence — ESP-IDF side.
 *
 * See nvs.h for the wiring order and ownership boundaries. Pure-C
 * serialization and sanitization live in nvs_logic.c so they can be
 * unit-tested on the host.
 */

#include "nvs.h"
#include "nvs_logic.h"
#include "state_bus.h"
#include "state_bus_events.h"

#include <string.h>

#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "nvs.h"
#include "nvs_flash.h"

static const char *TAG = "nvs";

#define LL_NVS_NAMESPACE     "ll_settings"
#define LL_NVS_KEY_STATE     "state"
#define LL_NVS_DEBOUNCE_US   (2 * 1000 * 1000)  /* 2 seconds */

static esp_timer_handle_t g_save_timer;
static ll_state_t g_pending_state;
static SemaphoreHandle_t g_pending_lock;
static bool g_pending_valid;

static esp_err_t write_state_blob(const ll_state_t *s)
{
    ll_nvs_blob_t blob;
    ll_nvs_serialize(s, &blob);

    nvs_handle_t h;
    esp_err_t err = nvs_open(LL_NVS_NAMESPACE, NVS_READWRITE, &h);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_open(RW) failed: %s", esp_err_to_name(err));
        return err;
    }

    err = nvs_set_blob(h, LL_NVS_KEY_STATE, &blob, sizeof(blob));
    if (err == ESP_OK) {
        err = nvs_commit(h);
    }
    nvs_close(h);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "save failed: %s", esp_err_to_name(err));
    }
    return err;
}

static esp_err_t erase_namespace(void)
{
    nvs_handle_t h;
    esp_err_t err = nvs_open(LL_NVS_NAMESPACE, NVS_READWRITE, &h);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return ESP_OK;  /* nothing to erase */
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_open for erase failed: %s", esp_err_to_name(err));
        return err;
    }
    err = nvs_erase_all(h);
    if (err == ESP_OK) {
        err = nvs_commit(h);
    }
    nvs_close(h);
    return err;
}

static void save_timer_cb(void *arg)
{
    (void)arg;
    ll_state_t snapshot;
    bool valid = false;

    if (xSemaphoreTake(g_pending_lock, portMAX_DELAY) == pdTRUE) {
        if (g_pending_valid) {
            snapshot = g_pending_state;
            g_pending_valid = false;
            valid = true;
        }
        xSemaphoreGive(g_pending_lock);
    }

    if (valid) {
        write_state_blob(&snapshot);
    }
}

static void schedule_debounced_save(const ll_state_t *s)
{
    if (xSemaphoreTake(g_pending_lock, portMAX_DELAY) == pdTRUE) {
        g_pending_state = *s;
        g_pending_valid = true;
        xSemaphoreGive(g_pending_lock);
    }
    /* Restarting the timer coalesces bursts of changes into a single
     * flash write after LL_NVS_DEBOUNCE_US of quiet. */
    esp_timer_stop(g_save_timer);
    esp_timer_start_once(g_save_timer, LL_NVS_DEBOUNCE_US);
}

static void on_state_changed(void *arg, esp_event_base_t base,
                             int32_t id, void *data)
{
    (void)arg; (void)base; (void)id;
    const ll_state_changed_payload_t *p = data;

    if (p->which == LL_EV_FACTORY_RESET) {
        /* State has been reset to defaults by state_bus; wipe the flash
         * blob so the next boot falls back to defaults naturally instead
         * of reading a persisted-defaults blob. Cancel any pending save
         * first — we don't want it to race with the erase. */
        esp_timer_stop(g_save_timer);
        if (xSemaphoreTake(g_pending_lock, portMAX_DELAY) == pdTRUE) {
            g_pending_valid = false;
            xSemaphoreGive(g_pending_lock);
        }
        esp_err_t err = erase_namespace();
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "factory reset: settings namespace erased");
        }
        return;
    }

    if (p->which == LL_EV_PROVISION_START) {
        /* Not a settings change; no save needed. */
        return;
    }

    schedule_debounced_save(ll_state_bus_get());
}

esp_err_t ll_nvs_init(ll_state_t *out, bool *first_boot_out)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        /* Partition is corrupt or was formatted by a newer IDF. Wipe and
         * retry — we treat both as "first boot" for settings purposes. */
        ESP_LOGW(TAG, "nvs_flash_init: %s — erasing partition",
                 esp_err_to_name(err));
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    if (err != ESP_OK) {
        return err;
    }

    /* Load persisted state, falling back to defaults on any non-OK path. */
    bool loaded = false;
    nvs_handle_t h;
    err = nvs_open(LL_NVS_NAMESPACE, NVS_READONLY, &h);
    if (err == ESP_OK) {
        ll_nvs_blob_t blob;
        size_t size = sizeof(blob);
        err = nvs_get_blob(h, LL_NVS_KEY_STATE, &blob, &size);
        if (err == ESP_OK && ll_nvs_deserialize(&blob, size, out)) {
            loaded = true;
        } else if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGW(TAG, "nvs_get_blob: %s — using defaults",
                     esp_err_to_name(err));
        }
        nvs_close(h);
    } else if (err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "nvs_open(RO): %s — using defaults",
                 esp_err_to_name(err));
    }

    if (!loaded) {
        ll_state_defaults(out);
    }
    if (first_boot_out) {
        *first_boot_out = !loaded;
    }

    /* Prepare the debounce timer and pending-state mutex up front so
     * ll_nvs_subscribe() can start posting into them immediately. */
    g_pending_lock = xSemaphoreCreateMutex();
    if (!g_pending_lock) {
        return ESP_ERR_NO_MEM;
    }

    const esp_timer_create_args_t targs = {
        .callback = &save_timer_cb,
        .name = "ll_nvs_save",
    };
    err = esp_timer_create(&targs, &g_save_timer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_timer_create: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "%s: on=%d pattern=%s color=#%06lX brightness=%u leds=%u",
             loaded ? "loaded" : "defaults (first boot)",
             (int)out->on, out->pattern_id,
             (unsigned long)out->base_color_rgb,
             (unsigned)out->brightness, (unsigned)out->led_count);
    return ESP_OK;
}

esp_err_t ll_nvs_subscribe(void)
{
    return esp_event_handler_instance_register_with(
        ll_state_bus_get_loop(),
        LL_STATE_EVENT_BASE, LL_EV_STATE_CHANGED,
        on_state_changed, NULL, NULL);
}

void ll_nvs_flush(void)
{
    ll_state_t snapshot;
    bool valid = false;

    if (xSemaphoreTake(g_pending_lock, portMAX_DELAY) == pdTRUE) {
        if (g_pending_valid) {
            snapshot = g_pending_state;
            g_pending_valid = false;
            valid = true;
        }
        xSemaphoreGive(g_pending_lock);
    }
    if (valid) {
        esp_timer_stop(g_save_timer);
        write_state_blob(&snapshot);
    }
}
