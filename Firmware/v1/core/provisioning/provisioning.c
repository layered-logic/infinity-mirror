/*
 * Wi-Fi provisioning + station lifecycle — ESP-IDF side.
 *
 * V1 provisioning surface (Apr 2026):
 *   - First-boot path: no creds in NVS → open a plain SoftAP
 *     "LL-Mirror-<3-byte MAC suffix>" at 192.168.4.1. Captive-portal
 *     DNS hijack (core/captive_dns/) routes the host OS's probe URLs
 *     to the device, which auto-pops the embedded webapp's /setup
 *     screen. The user enters SSID + password, the webapp submits
 *     them via the JSON `set_wifi_creds` op (core/transport/), and
 *     on_wifi_apply_creds below performs the SoftAP→STA handoff.
 *   - Cred-application failure path: 15s g_apply_fallback_timer; if
 *     STA hasn't connected within that window, esp_wifi_restore wipes
 *     the bad creds and bring_up_softap_runtime restores the SoftAP
 *     so the user can retry without a factory reset.
 *   - Saved-creds boot path: NVS has creds → bring up STA mode and
 *     attempt connect. Stays in retry-forever mode if the network is
 *     temporarily unavailable (so the device reconnects when wifi
 *     comes back). Recessed-button factory reset is the recovery for
 *     creds that are no longer valid.
 *
 * BLE provisioning is deferred to a post-V1 stretch goal (see
 * mini-sprint-app-demo.md §4). When it lands, the wifi_prov_mgr
 * scaffolding still imported below gains a real consumer:
 * wifi_prov_scheme_ble running alongside this SoftAP path. RN app
 * uses Espressif's ESPProvision SDK for that flow; webapp keeps its
 * SoftAP-only role for users who don't want to install a phone app.
 *
 * SoftAP SSID: "LL-Mirror-<3-byte MAC suffix hex>". Uniqueness per
 * device without secret distribution. Open auth — paired-mode HMAC
 * envelope is post-pairing concern, not provisioning concern.
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

#include "ll_wifi.h"
#include "state_bus.h"
#include "state_bus_events.h"

static const char *TAG = "provisioning";

#define LL_PROV_TIMEOUT_US         ((int64_t)5 * 60 * 1000 * 1000)  /* 5 minutes */
#define LL_PROV_POP                "layered-logic"                  /* TODO: MAC-derived */
#define LL_PROV_SSID_PREFIX        "LL-Setup-"
#define LL_PROV_MAX_CONNECT_RETRY  10   /* retries before we post DISCONNECTED */

/* SoftAP-based provisioning: when no Wi-Fi creds are saved at boot,
 * open a plain SoftAP at 192.168.4.1 so the embedded webapp's /setup
 * screen is reachable. Stays up indefinitely (no 5-min window) — the
 * wifi_prov_mgr-style timeout is handled at the cred-apply layer
 * (g_apply_fallback_timer) rather than at the SoftAP-availability
 * layer.
 *
 * Build flag for unusual deployments (e.g., a future BLE-only or
 * Matter-only variant where SoftAP isn't the user-facing surface):
 * pass `-DLL_SOFTAP_PROVISIONING=0` to skip the SoftAP path, leaving
 * the device "radios dark" until something explicitly posts
 * LL_EV_PROVISION_START. Default is ON for the V1 standard variant. */
#ifndef LL_SOFTAP_PROVISIONING
#define LL_SOFTAP_PROVISIONING     1
#endif

#if LL_SOFTAP_PROVISIONING
#define LL_AP_SSID_PREFIX          "LL-Mirror-"
#define LL_AP_CHANNEL              1
#define LL_AP_MAX_CONN             4
#endif

static esp_timer_handle_t g_prov_timeout_timer;
static esp_timer_handle_t g_apply_fallback_timer;
static bool g_prov_active;
static uint8_t g_retry_count;
static bool g_station_started;
static bool g_last_posted_connected;
static char g_sta_ssid[33];      /* current STA network SSID, empty when not on STA */
static char g_sta_ip[16];        /* current STA IPv4 dotted-quad, empty when not on STA */
static char g_pending_ssid[33];  /* ssid being applied via LL_EV_WIFI_APPLY_CREDS;
                                    cleared on STA up, removed from ll_wifi on fallback */

#define LL_APPLY_FALLBACK_US  ((int64_t)15 * 1000 * 1000)  /* 15s STA-join window before falling back */

static void build_ap_ssid(char *out, size_t out_len)
{
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);
    snprintf(out, out_len, LL_PROV_SSID_PREFIX "%02X%02X%02X",
             mac[3], mac[4], mac[5]);
}

/* ---- ll_wifi bridge helpers ---------------------------------------------- *
 * Step 2 of LL-046: provisioning now drives STA from the ll_wifi store
 * instead of esp_wifi's saved cred. ll_wifi is the source of truth;
 * esp_wifi_set_storage(WIFI_STORAGE_RAM) ensures we don't double-write.
 */

static int find_idx_by_ssid(const char *ssid)
{
    if (ssid == NULL) return -1;
    ll_wifi_list_t snapshot;
    ll_wifi_get_list(&snapshot);
    for (uint8_t i = 0; i < snapshot.count; i++) {
        if (strncmp(snapshot.entries[i].ssid, ssid, LL_WIFI_SSID_MAX) == 0) {
            return (int)i;
        }
    }
    return -1;
}

/* Push entries[idx]'s creds into esp_wifi as the STA config. Caller is
 * expected to call esp_wifi_set_mode(STA) and esp_wifi_start() around it. */
static esp_err_t apply_entry_to_esp_wifi(uint8_t idx)
{
    ll_wifi_list_t snapshot;
    ll_wifi_get_list(&snapshot);
    if (idx >= snapshot.count) {
        return ESP_ERR_INVALID_ARG;
    }

    wifi_config_t scfg = {0};
    strncpy((char *)scfg.sta.ssid,
            snapshot.entries[idx].ssid, sizeof(scfg.sta.ssid) - 1);
    strncpy((char *)scfg.sta.password,
            snapshot.entries[idx].password, sizeof(scfg.sta.password) - 1);
    return esp_wifi_set_config(WIFI_IF_STA, &scfg);
}

/* Pick the entry to attempt at boot. Prefer the previously-active one
 * (that's the network the device was on when it was last powered down,
 * the most natural reconnect target); fall back to "highest last_used"
 * for the cold-start case. Returns -1 only if the list is empty.
 *
 * Step 3 will replace this with the SCANNING → PICKING → BACKOFF state
 * machine that scans first and picks based on visibility. */
static int pick_boot_idx(void)
{
    uint8_t a = ll_wifi_get_active_idx();
    if (a != LL_WIFI_ACTIVE_NONE) {
        return (int)a;
    }
    ll_wifi_list_t snapshot;
    ll_wifi_get_list(&snapshot);
    return ll_wifi_list_pick_next(&snapshot, 0);
}

/* If ll_wifi is empty but esp_wifi has a saved STA cred from the legacy
 * single-network era, copy it over (§5.3 of multi-network-design.md).
 * Run AFTER esp_wifi_init (so esp_wifi has loaded the saved cred from
 * NVS) and AFTER ll_wifi_init. */
static void migrate_legacy_cred_if_needed(void)
{
    if (ll_wifi_count() != 0) {
        return;
    }

    wifi_config_t cur = {0};
    if (esp_wifi_get_config(WIFI_IF_STA, &cur) != ESP_OK) {
        return;
    }
    if (cur.sta.ssid[0] == '\0') {
        return;  /* never provisioned in the legacy era either */
    }

    char ssid[33] = {0};
    char password[65] = {0};
    strncpy(ssid, (const char *)cur.sta.ssid, sizeof(ssid) - 1);
    strncpy(password, (const char *)cur.sta.password, sizeof(password) - 1);

    esp_err_t err = ll_wifi_migrate_from_esp_wifi(ssid, password);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "migration: legacy esp_wifi cred (\"%s\") → ll_wifi[0]",
                 ssid);
    } else {
        ESP_LOGW(TAG, "migration failed: %s", esp_err_to_name(err));
    }
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

    /* Cache for the wire state's `wifi_ssid` and `ip` fields. */
    strncpy(g_sta_ssid, payload.ssid, sizeof(g_sta_ssid) - 1);
    g_sta_ssid[sizeof(g_sta_ssid) - 1] = '\0';
    snprintf(g_sta_ip, sizeof(g_sta_ip), IPSTR, IP2STR(&ip_info->ip));

    /* Cancel any in-flight cred-apply fallback — STA is up, the new
     * credentials worked. esp_timer_stop is safe on a not-running
     * timer (returns ESP_ERR_INVALID_STATE which we ignore). */
    esp_timer_stop(g_apply_fallback_timer);

    /* Stamp ll_wifi: this entry is now the active one, last_used = now.
     * Used by the boot-time pick_boot_idx() and (future Step 3) the
     * scan-and-pick "highest last_used wins" logic. */
    int idx = find_idx_by_ssid(payload.ssid);
    if (idx >= 0) {
        ll_wifi_set_active((uint8_t)idx, esp_timer_get_time());
    } else {
        /* Got an IP on a network not in ll_wifi — should not happen
         * since we always apply via ll_wifi-driven set_config. Don't
         * crash; just log and let the connection live untracked. */
        ESP_LOGW(TAG, "connected to \"%s\" but it is not in ll_wifi",
                 payload.ssid);
    }
    g_pending_ssid[0] = '\0';

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
    /* ll_wifi is the source of truth — wipe it. esp_wifi_restore on the
     * legacy NVS namespace is belt-and-suspenders for devices that were
     * provisioned in the single-cred era; harmless once storage is RAM. */
    esp_err_t err = ll_wifi_erase_all();
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "ll_wifi_erase_all: %s", esp_err_to_name(err));
    }
    err = esp_wifi_restore();
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "esp_wifi_restore: %s", esp_err_to_name(err));
    }
    g_pending_ssid[0] = '\0';
    post_wifi_disconnected(0);
}

/*
 * Async SoftAP→STA handoff. transport.c posts LL_EV_WIFI_APPLY_CREDS
 * after the JSON `set_wifi_creds` op response is queued; this handler
 * runs on the state-bus task, so the response has time to flush before
 * the underlying SoftAP socket dies.
 *
 * As of LL-046 Step 2: ll_wifi is the source of truth. We persist via
 * ll_wifi_add (insert-or-update by SSID), then push that entry's creds
 * into esp_wifi as the active STA config. esp_wifi storage is RAM —
 * set in ll_provisioning_init — so set_config doesn't double-write.
 */
static void on_wifi_apply_creds(void *arg, esp_event_base_t base,
                                int32_t id, void *data)
{
    (void)arg; (void)base; (void)id;
    const ll_ev_wifi_apply_creds_payload_t *p = data;

    /* Persist to ll_wifi first — even if the rest of this handler
     * fails, the cred is saved and a reboot will re-attempt at boot.
     * Insert-or-update; on full-list, surface the error and bail. The
     * webapp doesn't yet handle WIFI_LIST_FULL gracefully (Step 4 of
     * LL-046 introduces an explicit `add_wifi_network` op with proper
     * error reporting on the wire). */
    ll_wifi_add_result_t add_r = ll_wifi_add(p->ssid, p->password);
    if (add_r == LL_WIFI_ADD_ERR_FULL) {
        ESP_LOGE(TAG, "ll_wifi list full — cannot add \"%s\"", p->ssid);
        return;
    }
    if (add_r == LL_WIFI_ADD_ERR_INVALID) {
        ESP_LOGE(TAG, "ll_wifi rejected \"%s\" (invalid ssid/password)",
                 p->ssid);
        return;
    }

    /* Track which ssid is in flight so the apply-creds fallback can
     * remove it from ll_wifi if STA never comes up within the window. */
    strncpy(g_pending_ssid, p->ssid, sizeof(g_pending_ssid) - 1);
    g_pending_ssid[sizeof(g_pending_ssid) - 1] = '\0';

    /* Brief yield so the httpd task gets to flush the JSON ok=true
     * response before we tear down the SoftAP underneath it. Without
     * this, the WS frame is queued in lwip but the netif dies before
     * TCP can push it out — the webapp's submit promise hangs and
     * never enters the "credentials sent" view, even though the
     * firmware-side handoff worked. 250ms is generous; typical send
     * latency on the SoftAP is single-digit ms. */
    vTaskDelay(pdMS_TO_TICKS(250));

    ESP_LOGI(TAG, "applying creds: ssid=\"%s\" pw_len=%u (%s)",
             p->ssid, (unsigned)strlen(p->password),
             add_r == LL_WIFI_ADD_OK_INSERTED ? "new" : "updated");

    /* Tear down whatever wifi mode we're currently in (SoftAP, almost
     * always) before switching to STA. esp_wifi_stop closes the AP and
     * disconnects any connected clients — the JSON response was already
     * queued before this handler ran, so the webapp will see "socket
     * closed" and fall into reconnect-with-backoff. */
    esp_wifi_stop();

    esp_err_t err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "set_mode STA: %s", esp_err_to_name(err));
        return;
    }

    /* Look up the entry we just added/updated and push it into esp_wifi. */
    int idx = find_idx_by_ssid(p->ssid);
    if (idx < 0) {
        ESP_LOGE(TAG, "post-add lookup failed for \"%s\"", p->ssid);
        return;
    }
    err = apply_entry_to_esp_wifi((uint8_t)idx);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "apply_entry_to_esp_wifi: %s", esp_err_to_name(err));
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
     * fires, the new ll_wifi entry gets removed and the SoftAP comes
     * back up so the user can retry — protecting against typo'd SSIDs
     * and wrong passwords. post_wifi_connected() cancels the timer on
     * success. */
    esp_timer_start_once(g_apply_fallback_timer, LL_APPLY_FALLBACK_US);

    /* WIFI_EVENT_STA_START → on_wifi_event → esp_wifi_connect(). On
     * IP_EVENT_STA_GOT_IP we post LL_EV_WIFI_CONNECTED with the new
     * SSID/IP, which mdns republishes on and transport rebroadcasts
     * the wire state for. */
}

#if LL_SOFTAP_PROVISIONING
/* SoftAP helpers ----------------------------------------------------
 *
 * Posts LL_EV_WIFI_CONNECTED on WIFI_EVENT_AP_START so that the rest
 * of the system (transport, mdns, captive_dns) reacts the same way as
 * it would for a real STA connection. The payload's "ssid" field is
 * reused for the AP SSID; "ip" is the well-known SoftAP gateway
 * 192.168.4.1.
 */
static void on_ap_started(void *arg, esp_event_base_t base,
                              int32_t id, void *data)
{
    (void)arg; (void)base; (void)id; (void)data;

    wifi_config_t cfg;
    if (esp_wifi_get_config(WIFI_IF_AP, &cfg) != ESP_OK) {
        ESP_LOGW(TAG, "ap_start: cannot read AP config");
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

    ESP_LOGI(TAG, "SoftAP up: ssid=\"%s\" ip=192.168.4.1 (open, %d max clients)",
             payload.ssid, LL_AP_MAX_CONN);
}

static bool g_softap_init_done;

/* Configure the AP and register the AP_START handler — but do NOT
 * call esp_wifi_start. The actual start is deferred to
 * ll_provisioning_kick_softap, which main.c runs after every
 * subscriber (mdns, transport, etc.) has registered. Otherwise the
 * AP comes up so fast that LL_EV_WIFI_CONNECTED fires before anyone
 * is listening, and downstream modules silently miss it. */
static esp_err_t init_softap(void)
{
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);

    wifi_config_t cfg = {0};
    snprintf((char *)cfg.ap.ssid, sizeof(cfg.ap.ssid),
             LL_AP_SSID_PREFIX "%02X%02X%02X",
             mac[3], mac[4], mac[5]);
    cfg.ap.ssid_len       = strlen((const char *)cfg.ap.ssid);
    cfg.ap.channel        = LL_AP_CHANNEL;
    cfg.ap.authmode       = WIFI_AUTH_OPEN;
    cfg.ap.max_connection = LL_AP_MAX_CONN;

    esp_err_t err = esp_event_handler_instance_register(
        WIFI_EVENT, WIFI_EVENT_AP_START, on_ap_started, NULL, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ap_start handler register: %s", esp_err_to_name(err));
        return err;
    }

    err = esp_wifi_set_mode(WIFI_MODE_AP);
    if (err != ESP_OK) return err;
    err = esp_wifi_set_config(WIFI_IF_AP, &cfg);
    if (err != ESP_OK) return err;

    g_softap_init_done = true;
    ESP_LOGI(TAG, "SoftAP configured (\"%s\", open, no password) - "
                  "deferred until subscribers wired",
             cfg.ap.ssid);
    return ESP_OK;
}

/* Bring the SoftAP back up at runtime — used by the cred-apply
 * fallback when set_wifi_creds receives an SSID we can't actually join.
 * Re-applies the AP config (cleared when we switched to STA mode in
 * on_wifi_apply_creds) and starts wifi. The existing AP_START handler
 * registered by init_softap() reposts LL_EV_WIFI_CONNECTED with the
 * SoftAP gateway, so transport / mdns / captive_dns all come back up
 * just like at boot. */
static esp_err_t bring_up_softap_runtime(void)
{
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);

    wifi_config_t cfg = {0};
    snprintf((char *)cfg.ap.ssid, sizeof(cfg.ap.ssid),
             LL_AP_SSID_PREFIX "%02X%02X%02X",
             mac[3], mac[4], mac[5]);
    cfg.ap.ssid_len       = strlen((const char *)cfg.ap.ssid);
    cfg.ap.channel        = LL_AP_CHANNEL;
    cfg.ap.authmode       = WIFI_AUTH_OPEN;
    cfg.ap.max_connection = LL_AP_MAX_CONN;

    esp_err_t err = esp_wifi_set_mode(WIFI_MODE_AP);
    if (err != ESP_OK) return err;
    err = esp_wifi_set_config(WIFI_IF_AP, &cfg);
    if (err != ESP_OK) return err;
    err = esp_wifi_start();
    if (err != ESP_OK) return err;

    ESP_LOGW(TAG, "fallback: SoftAP restarted, awaiting fresh creds");
    return ESP_OK;
}
#endif /* LL_SOFTAP_PROVISIONING */

/* Cred-apply fallback: the STA join didn't complete within the window.
 * Bogus SSID, wrong password, network out of range — same fix applies:
 * remove the just-added entry from ll_wifi so we don't auto-retry it on
 * the next boot, and bring the SoftAP back up so the user can re-enter
 * fresh creds from /setup.
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

    /* Remove the failed entry from ll_wifi (source of truth). */
    if (g_pending_ssid[0] != '\0') {
        ll_wifi_remove_result_t r = ll_wifi_remove(g_pending_ssid);
        if (r == LL_WIFI_REMOVE_OK) {
            ESP_LOGW(TAG, "fallback: removed \"%s\" from ll_wifi",
                     g_pending_ssid);
        }
        g_pending_ssid[0] = '\0';
    }

    esp_wifi_disconnect();
    esp_wifi_stop();
    /* esp_wifi_restore() is largely vestigial post Step 2 (storage is
     * RAM, ll_wifi is the source of truth) but kept as belt-and-
     * suspenders for any cred that legacy code paths may have written
     * before the migration. */
    esp_wifi_restore();

    g_station_started = false;
    g_last_posted_connected = false;
    g_sta_ssid[0] = '\0';
    g_sta_ip[0] = '\0';
    g_retry_count = 0;

#if LL_SOFTAP_PROVISIONING
    if (bring_up_softap_runtime() != ESP_OK) {
        ESP_LOGE(TAG, "fallback: bring_up_softap_runtime failed - "
                      "device is unreachable, recessed-hold factory reset required");
    }
#else
    /* Build flag opted out of SoftAP provisioning (e.g., a future
     * BLE-only or Matter-only variant). Caller is responsible for
     * triggering whatever provisioning surface they wired up — there's
     * no "rescue path" we can run unsupervised here. */
    ESP_LOGE(TAG, "fallback: LL_SOFTAP_PROVISIONING=0 build, no SoftAP to "
                  "restore - device is unreachable, recessed-hold factory "
                  "reset required");
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

    /* Multi-network store: load saved networks, migrate any legacy
     * single-cred from esp_wifi's NVS namespace, then take ownership
     * of the credential pipeline. After this, ll_wifi is the source
     * of truth — set storage to RAM so subsequent set_config calls
     * don't keep writing back into esp_wifi's NVS. */
    err = ll_wifi_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ll_wifi_init: %s", esp_err_to_name(err));
        return err;
    }
    migrate_legacy_cred_if_needed();
    err = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    if (err != ESP_OK) {
        /* Non-fatal: storage stays at default FLASH, which produces a
         * harmless duplicate cache in esp_wifi's NVS. ll_wifi remains
         * authoritative on read. */
        ESP_LOGW(TAG, "set_storage RAM: %s — continuing with default",
                 esp_err_to_name(err));
    }

    if (ll_wifi_count() > 0) {
        int idx = pick_boot_idx();
        if (idx < 0) {
            /* count > 0 but pick failed — should not happen. Fall back
             * to entry 0 rather than dropping into provisioning. */
            idx = 0;
        }
        ESP_LOGI(TAG, "ll_wifi has %u entries — starting STA on idx=%d",
                 (unsigned)ll_wifi_count(), idx);
        err = esp_wifi_set_mode(WIFI_MODE_STA);
        if (err != ESP_OK) return err;
        err = apply_entry_to_esp_wifi((uint8_t)idx);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "boot apply_entry: %s", esp_err_to_name(err));
            return err;
        }
        err = esp_wifi_start();
        if (err != ESP_OK) return err;
        g_station_started = true;
    } else {
        ESP_LOGI(TAG, "ll_wifi empty — radios dark (awaiting LL_EV_PROVISION_START)");
#if LL_SOFTAP_PROVISIONING
        err = init_softap();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "SoftAP init failed: %s", esp_err_to_name(err));
            return err;
        }
#endif
    }

    return ESP_OK;
}

esp_err_t ll_provisioning_kick_softap(void)
{
#if LL_SOFTAP_PROVISIONING
    if (!g_softap_init_done) return ESP_OK;
    esp_err_t err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "SoftAP esp_wifi_start: %s", esp_err_to_name(err));
        return err;
    }
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

const char *ll_provisioning_get_sta_ip(void)
{
    return g_sta_ip;
}
