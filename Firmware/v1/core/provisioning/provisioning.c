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

/* DEV-ONLY: when no Wi-Fi creds are saved, open a plain SoftAP at
 * boot (no provisioning protocol on top) so transport + mdns are
 * reachable from any wifi client without going through the full
 * wifi_prov_mgr dance. Stays up indefinitely — no 5-min window —
 * because Sessions 1-4 want a stable network for iterative testing.
 *
 * Replaced by the real captive-portal flow in Session 5 of the app
 * demo mini-sprint. Until then, builds default to ON; flip to 0
 * (or pass `-DLL_DEV_OPEN_SOFTAP=0` at build time) to test the
 * production "radios dark" boot path. MUST be 0 for any production
 * release. */
#ifndef LL_DEV_OPEN_SOFTAP
#define LL_DEV_OPEN_SOFTAP         1
#endif

#if LL_DEV_OPEN_SOFTAP
#define LL_DEV_AP_SSID_PREFIX      "LL-Mirror-"
#define LL_DEV_AP_CHANNEL          1
#define LL_DEV_AP_MAX_CONN         4
#endif

static esp_timer_handle_t g_prov_timeout_timer;
static esp_timer_handle_t g_apply_fallback_timer;
static bool g_prov_active;
static uint8_t g_retry_count;
static bool g_station_started;
static bool g_last_posted_connected;
static char g_sta_ssid[33];   /* current STA network SSID, empty when not on STA */

#define LL_APPLY_FALLBACK_US  ((int64_t)15 * 1000 * 1000)  /* 15s STA-join window before falling back */

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
    g_sta_ssid[0] = '\0';
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

    /* Cache for the wire state's `wifi_ssid` field. */
    strncpy(g_sta_ssid, payload.ssid, sizeof(g_sta_ssid) - 1);
    g_sta_ssid[sizeof(g_sta_ssid) - 1] = '\0';

    /* Cancel any in-flight cred-apply fallback — STA is up, the new
     * credentials worked. esp_timer_stop is safe on a not-running
     * timer (returns ESP_ERR_INVALID_STATE which we ignore). */
    esp_timer_stop(g_apply_fallback_timer);

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

    ESP_LOGW(TAG, "factory reset - wiping Wi-Fi credentials");
    esp_wifi_disconnect();
    /* esp_wifi_restore() clears the saved sta config in NVS. */
    esp_err_t err = esp_wifi_restore();
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "esp_wifi_restore: %s", esp_err_to_name(err));
    }
    post_wifi_disconnected(0);
}

/*
 * Async SoftAP→STA handoff. transport.c posts LL_EV_WIFI_APPLY_CREDS
 * after the JSON `set_wifi_creds` op response is queued; this handler
 * runs on the state-bus task, so the response has time to flush before
 * the underlying SoftAP socket dies.
 *
 * Storage = FLASH so creds survive reboot — the dev hardcoded-creds
 * spike used WIFI_STORAGE_RAM intentionally to skip this. In production
 * this is the only place creds enter NVS, since wifi_prov_mgr's
 * protocomm path is not driven for the SoftAP scheme in V1.
 */
static void on_wifi_apply_creds(void *arg, esp_event_base_t base,
                                int32_t id, void *data)
{
    (void)arg; (void)base; (void)id;
    const ll_ev_wifi_apply_creds_payload_t *p = data;

    /* Brief yield so the httpd task gets to flush the JSON ok=true
     * response before we tear down the SoftAP underneath it. Without
     * this, the WS frame is queued in lwip but the netif dies before
     * TCP can push it out — the webapp's submit promise hangs and
     * never enters the "credentials sent" view, even though the
     * firmware-side handoff worked. 250ms is generous; typical send
     * latency on the SoftAP is single-digit ms. */
    vTaskDelay(pdMS_TO_TICKS(250));

    ESP_LOGI(TAG, "applying creds: ssid=\"%s\" pw_len=%u",
             p->ssid, (unsigned)strlen(p->password));

    esp_err_t err = esp_wifi_set_storage(WIFI_STORAGE_FLASH);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "set_storage: %s", esp_err_to_name(err));
        return;
    }

    /* Tear down whatever wifi mode we're currently in (SoftAP, almost
     * always) before switching to STA. esp_wifi_stop closes the AP and
     * disconnects any connected clients — the JSON response was already
     * queued before this handler ran, so the webapp will see "socket
     * closed" and fall into reconnect-with-backoff. */
    esp_wifi_stop();

    err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "set_mode STA: %s", esp_err_to_name(err));
        return;
    }

    wifi_config_t scfg = {0};
    strncpy((char *)scfg.sta.ssid,     p->ssid,     sizeof(scfg.sta.ssid) - 1);
    strncpy((char *)scfg.sta.password, p->password, sizeof(scfg.sta.password) - 1);
    err = esp_wifi_set_config(WIFI_IF_STA, &scfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "set_config STA: %s", esp_err_to_name(err));
        return;
    }

    err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "wifi_start STA: %s", esp_err_to_name(err));
        return;
    }
    g_station_started = true;
    g_retry_count = 0;

    /* Arm the fallback timer. If STA hasn't connected by the time it
     * fires, the bogus creds get wiped and the SoftAP comes back up so
     * the user can retry — protecting against typo'd SSIDs and wrong
     * passwords. post_wifi_connected() cancels the timer on success. */
    esp_timer_start_once(g_apply_fallback_timer, LL_APPLY_FALLBACK_US);

    /* WIFI_EVENT_STA_START → on_wifi_event → esp_wifi_connect(). On
     * IP_EVENT_STA_GOT_IP we post LL_EV_WIFI_CONNECTED with the new
     * SSID/IP, which mdns republishes on and transport rebroadcasts
     * the wire state for. */
}

#if LL_DEV_OPEN_SOFTAP
/* Dev SoftAP helpers ------------------------------------------------
 *
 * Posts LL_EV_WIFI_CONNECTED on WIFI_EVENT_AP_START so that the rest
 * of the system (transport, mdns) reacts the same way as it would for
 * a real STA connection. The payload's "ssid" field is reused for the
 * AP SSID; "ip" is the well-known SoftAP gateway 192.168.4.1.
 */
static void on_dev_ap_started(void *arg, esp_event_base_t base,
                              int32_t id, void *data)
{
    (void)arg; (void)base; (void)id; (void)data;

    wifi_config_t cfg;
    if (esp_wifi_get_config(WIFI_IF_AP, &cfg) != ESP_OK) {
        ESP_LOGW(TAG, "DEV ap_start: cannot read AP config");
        return;
    }

    ll_ev_wifi_connected_payload_t payload = {0};
    strncpy(payload.ssid, (const char *)cfg.ap.ssid, sizeof(payload.ssid) - 1);
    payload.ip[0] = 192; payload.ip[1] = 168;
    payload.ip[2] = 4;   payload.ip[3] = 1;
    payload.rssi = 0;

    esp_event_post_to(ll_state_bus_get_loop(), LL_STATE_EVENT_BASE,
                      LL_EV_WIFI_CONNECTED, &payload, sizeof(payload), 0);
    g_last_posted_connected = true;

    ESP_LOGI(TAG, "DEV SoftAP up: ssid=\"%s\" ip=192.168.4.1 (open, %d max clients)",
             payload.ssid, LL_DEV_AP_MAX_CONN);
}

static bool g_dev_softap_init_done;

/* Configure the AP and register the AP_START handler — but do NOT
 * call esp_wifi_start. The actual start is deferred to
 * ll_provisioning_kick_dev_softap, which main.c runs after every
 * subscriber (mdns, transport, etc.) has registered. Otherwise the
 * AP comes up so fast that LL_EV_WIFI_CONNECTED fires before anyone
 * is listening, and downstream modules silently miss it. */
static esp_err_t init_dev_softap(void)
{
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);

    wifi_config_t cfg = {0};
    snprintf((char *)cfg.ap.ssid, sizeof(cfg.ap.ssid),
             LL_DEV_AP_SSID_PREFIX "%02X%02X%02X",
             mac[3], mac[4], mac[5]);
    cfg.ap.ssid_len       = strlen((const char *)cfg.ap.ssid);
    cfg.ap.channel        = LL_DEV_AP_CHANNEL;
    cfg.ap.authmode       = WIFI_AUTH_OPEN;
    cfg.ap.max_connection = LL_DEV_AP_MAX_CONN;

    esp_err_t err = esp_event_handler_instance_register(
        WIFI_EVENT, WIFI_EVENT_AP_START, on_dev_ap_started, NULL, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "DEV ap_start handler register: %s", esp_err_to_name(err));
        return err;
    }

    err = esp_wifi_set_mode(WIFI_MODE_AP);
    if (err != ESP_OK) return err;
    err = esp_wifi_set_config(WIFI_IF_AP, &cfg);
    if (err != ESP_OK) return err;

    g_dev_softap_init_done = true;
    ESP_LOGI(TAG, "DEV SoftAP configured (\"%s\", open, no password) - "
                  "deferred until subscribers wired",
             cfg.ap.ssid);
    return ESP_OK;
}

/* Bring the dev SoftAP back up at runtime — used by the cred-apply
 * fallback when set_wifi_creds receives an SSID we can't actually join.
 * Re-applies the AP config (cleared when we switched to STA mode in
 * on_wifi_apply_creds) and starts wifi. The existing AP_START handler
 * registered by init_dev_softap() reposts LL_EV_WIFI_CONNECTED with the
 * SoftAP gateway, so transport / mdns / captive_dns all come back up
 * just like at boot. */
static esp_err_t bring_up_dev_softap_runtime(void)
{
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);

    wifi_config_t cfg = {0};
    snprintf((char *)cfg.ap.ssid, sizeof(cfg.ap.ssid),
             LL_DEV_AP_SSID_PREFIX "%02X%02X%02X",
             mac[3], mac[4], mac[5]);
    cfg.ap.ssid_len       = strlen((const char *)cfg.ap.ssid);
    cfg.ap.channel        = LL_DEV_AP_CHANNEL;
    cfg.ap.authmode       = WIFI_AUTH_OPEN;
    cfg.ap.max_connection = LL_DEV_AP_MAX_CONN;

    esp_err_t err = esp_wifi_set_mode(WIFI_MODE_AP);
    if (err != ESP_OK) return err;
    err = esp_wifi_set_config(WIFI_IF_AP, &cfg);
    if (err != ESP_OK) return err;
    err = esp_wifi_start();
    if (err != ESP_OK) return err;

    ESP_LOGW(TAG, "fallback: SoftAP restarted, awaiting fresh creds");
    return ESP_OK;
}
#endif /* LL_DEV_OPEN_SOFTAP */

/* Cred-apply fallback: the STA join didn't complete within the window.
 * Bogus SSID, wrong password, network out of range — same fix applies:
 * wipe the bad creds from NVS so they don't persist, and bring the
 * SoftAP back up so the user can retry from /setup.
 *
 * Without this, a single typo strands the device in failing-STA mode
 * forever (the existing on_wifi_event retry loop spins indefinitely
 * after the budget exhausts), recoverable only by recessed-button
 * factory reset. */
static void apply_creds_fallback_cb(void *arg)
{
    (void)arg;
    ESP_LOGW(TAG, "STA join did not complete in %ds - reverting creds, "
                  "returning to SoftAP for retry",
             (int)(LL_APPLY_FALLBACK_US / 1000000));

    esp_wifi_disconnect();
    esp_wifi_stop();
    esp_wifi_restore();   /* wipes saved sta config from NVS */

    g_station_started = false;
    g_last_posted_connected = false;
    g_sta_ssid[0] = '\0';
    g_retry_count = 0;

#if LL_DEV_OPEN_SOFTAP
    if (bring_up_dev_softap_runtime() != ESP_OK) {
        ESP_LOGE(TAG, "fallback: bring_up_dev_softap_runtime failed - "
                      "device is unreachable, recessed-hold factory reset required");
    }
#else
    /* Production path (sub-5+): re-trigger wifi_prov_mgr's SoftAP scheme
     * so the user gets the captive portal again. */
    ESP_LOGE(TAG, "fallback in non-dev build not yet implemented - "
                  "device is unreachable, recessed-hold factory reset required");
#endif
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

    const esp_timer_create_args_t fb_args = {
        .callback = &apply_creds_fallback_cb,
        .name = "ll_apply_fallback",
    };
    err = esp_timer_create(&fb_args, &g_apply_fallback_timer);
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
#if LL_DEV_OPEN_SOFTAP
        err = init_dev_softap();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "DEV SoftAP init failed: %s", esp_err_to_name(err));
            return err;
        }
#endif
    }

    return ESP_OK;
}

esp_err_t ll_provisioning_kick_dev_softap(void)
{
#if LL_DEV_OPEN_SOFTAP
    if (!g_dev_softap_init_done) return ESP_OK;
    esp_err_t err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "DEV SoftAP esp_wifi_start: %s", esp_err_to_name(err));
        return err;
    }
    ESP_LOGW(TAG, "DEV mode: SoftAP up. Captive-portal flow lands in "
                  "Session 5 of the app demo mini-sprint — replace before prod.");
    return ESP_OK;
#else
    return ESP_OK;
#endif
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
    if (err != ESP_OK) return err;

    return esp_event_handler_instance_register_with(
        ll_state_bus_get_loop(),
        LL_STATE_EVENT_BASE, LL_EV_WIFI_APPLY_CREDS,
        on_wifi_apply_creds, NULL, NULL);
}

bool ll_provisioning_is_active(void)
{
    /* AP-mode = "device is a SoftAP awaiting credentials." Covers both
     * the dev open-SoftAP path and (future) wifi_prov_mgr scheme paths.
     * STA / NULL = device is connected (or trying) and not provisioning. */
    wifi_mode_t mode;
    if (esp_wifi_get_mode(&mode) != ESP_OK) return false;
    return mode == WIFI_MODE_AP || mode == WIFI_MODE_APSTA;
}

const char *ll_provisioning_get_sta_ssid(void)
{
    return g_sta_ssid;
}
