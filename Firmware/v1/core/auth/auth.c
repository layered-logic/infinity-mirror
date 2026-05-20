/*
 * Paired-mode authentication — ESP-IDF side. See auth.h.
 *
 * NVS namespace `ll_auth`, key `shared_secret`, stored plaintext per
 * firmware-security.md §5.3. The pure-C HMAC + canonicalization is in
 * auth_logic.c (host-tested); this file holds the secret store, the RAM
 * cache, and the state-bus wiring.
 */

#include "auth.h"
#include "auth_logic.h"

#include <string.h>

#include "esp_event.h"
#include "esp_log.h"
#include "nvs.h"

#include "state_bus.h"
#include "state_bus_events.h"

static const char *TAG = "auth";

#define LL_AUTH_NAMESPACE  "ll_auth"
#define LL_AUTH_KEY_SECRET "shared_secret"

/* In-RAM copy of the secret so each incoming frame doesn't hit flash.
 * Empty string == no secret held. */
static char g_secret[LL_AUTH_SECRET_BUFSZ];

esp_err_t ll_auth_init(void)
{
    memset(g_secret, 0, sizeof(g_secret));

    nvs_handle_t h;
    esp_err_t err = nvs_open(LL_AUTH_NAMESPACE, NVS_READONLY, &h);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "no auth namespace — open mode");
        return ESP_OK;
    }
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "nvs_open(RO): %s — treating as no secret",
                 esp_err_to_name(err));
        return ESP_OK;
    }

    size_t len = sizeof(g_secret);
    err = nvs_get_str(h, LL_AUTH_KEY_SECRET, g_secret, &len);
    nvs_close(h);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "shared secret loaded");
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        /* Namespace exists (e.g. left by a prior erase) but no secret. */
    } else {
        ESP_LOGW(TAG, "nvs_get_str: %s — treating as no secret",
                 esp_err_to_name(err));
        memset(g_secret, 0, sizeof(g_secret));
    }
    return ESP_OK;
}

bool ll_auth_is_paired(void)
{
    return ll_state_bus_get()->auth_mode == LL_AUTH_PAIRED;
}

bool ll_auth_has_secret(void)
{
    return g_secret[0] != '\0';
}

esp_err_t ll_auth_set_secret(const char *secret)
{
    if (secret == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    size_t n = strlen(secret);
    if (n == 0 || n >= LL_AUTH_SECRET_BUFSZ) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t h;
    esp_err_t err = nvs_open(LL_AUTH_NAMESPACE, NVS_READWRITE, &h);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_open(RW): %s", esp_err_to_name(err));
        return err;
    }
    err = nvs_set_str(h, LL_AUTH_KEY_SECRET, secret);
    if (err == ESP_OK) {
        err = nvs_commit(h);
    }
    nvs_close(h);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "secret save failed: %s", esp_err_to_name(err));
        return err;
    }

    memset(g_secret, 0, sizeof(g_secret));
    memcpy(g_secret, secret, n);
    ESP_LOGI(TAG, "shared secret set");
    return ESP_OK;
}

esp_err_t ll_auth_clear_secret(void)
{
    memset(g_secret, 0, sizeof(g_secret));

    nvs_handle_t h;
    esp_err_t err = nvs_open(LL_AUTH_NAMESPACE, NVS_READWRITE, &h);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return ESP_OK;  /* nothing stored */
    }
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "nvs_open for clear: %s", esp_err_to_name(err));
        return err;
    }
    err = nvs_erase_all(h);
    if (err == ESP_OK) {
        err = nvs_commit(h);
    }
    nvs_close(h);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "shared secret cleared");
    }
    return err;
}

bool ll_auth_secret_matches(const char *candidate)
{
    if (candidate == NULL) {
        return false;
    }
    size_t held = strlen(g_secret);
    if (held == 0) {
        return false;  /* no secret stored */
    }
    /* Length is compared directly — that is not constant-time, but the
     * secret's length is not the sensitive part. The byte compare on an
     * equal-length candidate below runs in constant time. */
    if (held != strlen(candidate)) {
        return false;
    }
    return ll_consttime_eq((const uint8_t *)g_secret,
                           (const uint8_t *)candidate, held);
}

ll_auth_verdict_t ll_auth_verify(const char *raw_json, size_t len)
{
    if (!ll_auth_has_secret()) {
        /* Paired mode with no secret is a degenerate state — reject rather
         * than fail open. Recovery is a factory reset. */
        return LL_AUTH_NO_SECRET;
    }

    ll_sig_result_t r = ll_auth_check_signature(g_secret, strlen(g_secret),
                                                raw_json, len);
    switch (r) {
    case LL_SIG_OK:
        return LL_AUTH_OK;
    case LL_SIG_MISSING:
        return LL_AUTH_UNSIGNED;
    case LL_SIG_MALFORMED:
    case LL_SIG_MISMATCH:
    default:
        return LL_AUTH_BAD_HMAC;
    }
}

/* Factory reset wipes the shared secret. Mirrors the core/nvs/ subscriber:
 * the state-bus task re-dispatches LL_EV_FACTORY_RESET as an
 * LL_EV_STATE_CHANGED notification once the reset has been applied. Without
 * this, a user locked out of a paired device would have no recovery path. */
static void on_state_changed(void *arg, esp_event_base_t base,
                             int32_t id, void *data)
{
    (void)arg; (void)base; (void)id;
    const ll_state_changed_payload_t *p = data;
    if (p->which == LL_EV_FACTORY_RESET) {
        ll_auth_clear_secret();
    }
}

esp_err_t ll_auth_subscribe(void)
{
    return esp_event_handler_instance_register_with(
        ll_state_bus_get_loop(),
        LL_STATE_EVENT_BASE, LL_EV_STATE_CHANGED,
        on_state_changed, NULL, NULL);
}
