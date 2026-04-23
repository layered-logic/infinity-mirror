/*
 * Wi-Fi provisioning + station lifecycle — ESP-IDF side.
 *
 * Scheme choice (Apr 2026): SoftAP only. BLE is deferred until the mobile
 * app commits to a provisioning UX and we're willing to pay the ~60KB
 * nimble stack cost + sdkconfig flip (CONFIG_BT_ENABLED). SoftAP works
 * out of the box with the ESP SoftAP Provisioning app (iOS/Android) or a
 * plain phone Wi-Fi join + web flow, which is enough for device bring-up
 * and early user testing. When BLE lands, swap wifi_prov_scheme_softap for
 * wifi_prov_scheme_ble (both schemes implement the same manager API).
 *
 * Security: WIFI_PROV_SECURITY_1 with a hardcoded proof-of-possession.
 * A MAC-derived PoP + QR code on the product is the shipping plan; the
 * hardcoded PoP here is a dev-only shortcut marked TODO.
 *
 * SSID format: "LL-Setup-<3-byte MAC suffix hex>". Uniqueness per device
 * without requiring any secret distribution.
 */

#include "provisioning.h"

#include <string.h>

#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "wifi_provisioning/manager.h"
#include "wifi_provisioning/scheme_softap.h"

#include "state_bus.h"
#include "state_bus_events.h"

static const char *TAG = "provisioning";

#define LL_PROV_TIMEOUT_US         ((int64_t)5 * 60 * 1000 * 1000)  /* 5 minutes */
#define LL_PROV_POP                "layered-logic"                  /* TODO: MAC-derived */
#define LL_PROV_SSID_PREFIX        "LL-Setup-"
#define LL_PROV_MAX_CONNECT_RETRY  10   /* retries before we post DISCONNECTED */

static esp_timer_handle_t g_prov_timeout_timer;
static bool g_prov_active;
static uint8_t g_retry_count;
static bool g_station_started;
static bool g_last_posted_connected;

static void build_ap_ssid(char *out, size_t out_len)
{
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);
    snprintf(out, out_len, LL_PROV_SSID_PREFIX "%02X%02X%02X",
             mac[3], mac[4], mac[5]);
}

static void post_wifi_disconnected(uint8_t reason)
{
    if (!g_last_posted_connected) {
        /* Already in disconnected state; don't spam. */
        return;
    }
    g_last_posted_connected = false;
    ll_ev_wifi_disconnected_payload_t payload = { .reason = reason };
    esp_event_post_to(ll_state_bus_get_loop(), LL_STATE_EVENT_BASE,
                      LL_EV_WIFI_DISCONNECTED, &payload, sizeof(payload), 0);
}

static void post_wifi_connected(const esp_netif_ip_info_t *ip_info)
{
    wifi_ap_record_t ap = {0};
    esp_wifi_sta_get_ap_info(&ap);

    ll_ev_wifi_connected_payload_t payload = {0};
    strncpy(payload.ssid, (const char *)ap.ssid, sizeof(payload.ssid) - 1);
    payload.ip[0] = esp_ip4_addr1_16(&ip_info->ip);
    payload.ip[1] = esp_ip4_addr2_16(&ip_info->ip);
    payload.ip[2] = esp_ip4_addr3_16(&ip_info->ip);
    payload.ip[3] = esp_ip4_addr4_16(&ip_info->ip);
    payload.rssi = ap.rssi;

    g_last_posted_connected = true;
    esp_event_post_to(ll_state_bus_get_loop(), LL_STATE_EVENT_BASE,
                      LL_EV_WIFI_CONNECTED, &payload, sizeof(payload), 0);
}

static void on_wifi_event(void *arg, esp_event_base_t base,
                          int32_t id, void *data)
{
    (void)arg; (void)base;

    switch (id) {
    case WIFI_EVENT_STA_START:
        ESP_LOGI(TAG, "STA_START — attempting connect");
        esp_wifi_connect();
        break;

    case WIFI_EVENT_STA_CONNECTED:
        /* IP not assigned yet; wait for IP_EVENT_STA_GOT_IP before
         * posting LL_EV_WIFI_CONNECTED. Resetting the retry counter
         * here is fine since we'll re-increment on the next drop. */
        g_retry_count = 0;
        break;

    case WIFI_EVENT_STA_DISCONNECTED: {
        const wifi_event_sta_disconnected_t *d = data;
        if (g_retry_count < LL_PROV_MAX_CONNECT_RETRY) {
            g_retry_count++;
            ESP_LOGW(TAG, "STA_DISCONNECTED (reason=%u) — retry %u/%u",
                     (unsigned)d->reason, (unsigned)g_retry_count,
                     (unsigned)LL_PROV_MAX_CONNECT_RETRY);
            esp_wifi_connect();
        } else {
            ESP_LOGE(TAG, "STA_DISCONNECTED: retry budget exhausted (reason=%u)",
                     (unsigned)d->reason);
            post_wifi_disconnected(d->reason);
            /* Keep trying in the background — a flaky AP may come back.
             * Next success will post LL_EV_WIFI_CONNECTED and we reset
             * the counter. */
            g_retry_count = 0;
            esp_wifi_connect();
        }
        break;
    }

    default:
        break;
    }
}

static void on_ip_event(void *arg, esp_event_base_t base,
                        int32_t id, void *data)
{
    (void)arg; (void)base;

    if (id == IP_EVENT_STA_GOT_IP) {
        const ip_event_got_ip_t *e = data;
        ESP_LOGI(TAG, "got IP: " IPSTR, IP2STR(&e->ip_info.ip));
        post_wifi_connected(&e->ip_info);
    }
}

static void on_prov_event(void *arg, esp_event_base_t base,
                          int32_t id, void *data)
{
    (void)arg; (void)base;

    switch (id) {
    case WIFI_PROV_START:
        g_prov_active = true;
        ESP_LOGI(TAG, "provisioning started (SoftAP)");
        break;

    case WIFI_PROV_CRED_RECV: {
        const wifi_sta_config_t *cfg = data;
        ESP_LOGI(TAG, "received creds: ssid=%s", (const char *)cfg->ssid);
        break;
    }

    case WIFI_PROV_CRED_FAIL: {
        const wifi_prov_sta_fail_reason_t *reason = data;
        ESP_LOGE(TAG, "provisioning failed: reason=%d",
                 (int)*reason);
        wifi_prov_mgr_reset_sm_state_on_failure();
        break;
    }

    case WIFI_PROV_CRED_SUCCESS:
        ESP_LOGI(TAG, "provisioning succeeded");
        break;

    case WIFI_PROV_END:
        g_prov_active = false;
        esp_timer_stop(g_prov_timeout_timer);
        wifi_prov_mgr_deinit();
        ESP_LOGI(TAG, "provisioning ended");
        break;

    default:
        break;
    }
}

static void prov_timeout_cb(void *arg)
{
    (void)arg;
    if (g_prov_active) {
        ESP_LOGW(TAG, "provisioning window expired — stopping");
        wifi_prov_mgr_stop_provisioning();
        /* WIFI_PROV_END handler will clear g_prov_active + deinit. */
    }
}

static void on_provision_start(void *arg, esp_event_base_t base,
                               int32_t id, void *data)
{
    (void)arg; (void)base; (void)id; (void)data;

    if (g_prov_active) {
        ESP_LOGI(TAG, "provision start requested but already provisioning");
        return;
    }

    wifi_prov_mgr_config_t config = {
        .scheme = wifi_prov_scheme_softap,
        .scheme_event_handler = WIFI_PROV_EVENT_HANDLER_NONE,
    };
    esp_err_t err = wifi_prov_mgr_init(config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "wifi_prov_mgr_init: %s", esp_err_to_name(err));
        return;
    }

    char ssid[24];
    build_ap_ssid(ssid, sizeof(ssid));

    err = wifi_prov_mgr_start_provisioning(
        WIFI_PROV_SECURITY_1, LL_PROV_POP, ssid, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "wifi_prov_mgr_start_provisioning: %s",
                 esp_err_to_name(err));
        wifi_prov_mgr_deinit();
        return;
    }

    esp_timer_start_once(g_prov_timeout_timer, LL_PROV_TIMEOUT_US);
    ESP_LOGI(TAG, "advertising SoftAP \"%s\" (PoP=%s) — 5 min window",
             ssid, LL_PROV_POP);
}

static void on_factory_reset(void *arg, esp_event_base_t base,
                             int32_t id, void *data)
{
    (void)arg; (void)base; (void)id; (void)data;

    ESP_LOGW(TAG, "factory reset — wiping Wi-Fi credentials");
    esp_wifi_disconnect();
    /* esp_wifi_restore() clears the saved sta config in NVS. */
    esp_err_t err = esp_wifi_restore();
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "esp_wifi_restore: %s", esp_err_to_name(err));
    }
    post_wifi_disconnected(0);
}

esp_err_t ll_provisioning_init(void)
{
    /* netif + system event loop are prereqs for esp_wifi. nvs_flash is
     * already init'd by ll_nvs_init() upstream — esp_wifi reads creds
     * from it. */
    esp_err_t err = esp_netif_init();
    if (err != ESP_OK) return err;

    err = esp_event_loop_create_default();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) return err;

    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();  /* needed by SoftAP prov scheme */

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&cfg);
    if (err != ESP_OK) return err;

    err = esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, on_wifi_event, NULL, NULL);
    if (err != ESP_OK) return err;
    err = esp_event_handler_instance_register(
        IP_EVENT, ESP_EVENT_ANY_ID, on_ip_event, NULL, NULL);
    if (err != ESP_OK) return err;
    err = esp_event_handler_instance_register(
        WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, on_prov_event, NULL, NULL);
    if (err != ESP_OK) return err;

    const esp_timer_create_args_t targs = {
        .callback = &prov_timeout_cb,
        .name = "ll_prov_timeout",
    };
    err = esp_timer_create(&targs, &g_prov_timeout_timer);
    if (err != ESP_OK) return err;

    /* Check saved creds. Init prov mgr just long enough to query, then
     * deinit unless we're actively provisioning — keeps RAM low during
     * normal operation. */
    wifi_prov_mgr_config_t qcfg = {
        .scheme = wifi_prov_scheme_softap,
        .scheme_event_handler = WIFI_PROV_EVENT_HANDLER_NONE,
    };
    err = wifi_prov_mgr_init(qcfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "wifi_prov_mgr_init (query): %s", esp_err_to_name(err));
        return err;
    }

    bool provisioned = false;
    wifi_prov_mgr_is_provisioned(&provisioned);
    wifi_prov_mgr_deinit();

    if (provisioned) {
        ESP_LOGI(TAG, "creds present — starting station");
        err = esp_wifi_set_mode(WIFI_MODE_STA);
        if (err != ESP_OK) return err;
        err = esp_wifi_start();
        if (err != ESP_OK) return err;
        g_station_started = true;
    } else {
        ESP_LOGI(TAG, "no creds — radios dark (awaiting LL_EV_PROVISION_START)");
    }

    return ESP_OK;
}

esp_err_t ll_provisioning_subscribe(void)
{
    esp_err_t err = esp_event_handler_instance_register_with(
        ll_state_bus_get_loop(),
        LL_STATE_EVENT_BASE, LL_EV_PROVISION_START,
        on_provision_start, NULL, NULL);
    if (err != ESP_OK) return err;

    err = esp_event_handler_instance_register_with(
        ll_state_bus_get_loop(),
        LL_STATE_EVENT_BASE, LL_EV_FACTORY_RESET,
        on_factory_reset, NULL, NULL);
    return err;
}

bool ll_provisioning_is_active(void)
{
    return g_prov_active;
}
