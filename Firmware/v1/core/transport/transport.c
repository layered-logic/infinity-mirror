/*
 * HTTP + WebSocket transport — ESP-IDF side.
 *
 * Server lifecycle:
 *   wifi up   → httpd_start on port 80, register /ws URI handler
 *   wifi down → httpd_stop (active WS connections get closed)
 *
 * WS handler dispatches on the envelope's `op` field. Session 1
 * implements only `ping`; everything else returns
 *   { ok: false, error: { code: "unknown_op", ... } }
 * to keep clients honest about which version of the protocol they're
 * talking to.
 *
 * Threading: start_server / stop_server run on the state-bus event
 * loop (single task → no races). The WS handler runs on the httpd
 * task; it only reads g_server transitively via the framework's
 * sockfd table, never touches it directly. Open auth mode means no
 * shared mutable state to guard yet.
 */

#include "transport.h"

#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"
#include "esp_app_desc.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "ll_wifi.h"
#include "ota.h"
#include "provisioning.h"
#include "state_bus.h"
#include "state_bus_events.h"
#include "webapp_assets.h"

static const char *TAG = "transport";

#define LL_TRANSPORT_PORT  80
#define LL_WS_RX_MAX       1024   /* envelope+payload — generous for ping/state */

/* Debounce window for client broadcasts. Long enough to absorb a multi-
 * field set_state (event-bus dispatch is sub-ms even for 5-field payloads)
 * and tightly-clustered control clicks; short enough to be imperceptible
 * to users. Tune via empirical jitter testing if needed. */
#define LL_BROADCAST_DEBOUNCE_US (30 * 1000)

static httpd_handle_t     g_server;
static esp_timer_handle_t s_broadcast_timer;
static atomic_bool        s_broadcast_pending;

/* Forward decls — the multi-network ops below trigger broadcasts after
 * mutating the saved-list (so wifi_saved_count refreshes on connected
 * clients), but the body of broadcast_state and its helpers live
 * further down with the rest of the broadcast plumbing. */
static void broadcast_state(void);
static void do_broadcast_state(void);
static void broadcast_timer_cb(void *arg);

/* ---- state object serialization ---------------------------------- */

/* Wire-format state object per control-protocol-spec.md §5. Caller
 * takes ownership of the returned cJSON tree. */
static cJSON *state_to_json(const ll_state_t *s)
{
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;

    cJSON_AddBoolToObject  (obj, "on",         s->on);
    cJSON_AddStringToObject(obj, "pattern_id", s->pattern_id);

    /* base_color over the wire is "#RRGGBB" (hex string), not an int —
     * matches the spec example. Uppercase hex for legibility. */
    char hex[8];
    snprintf(hex, sizeof(hex), "#%06lX", (unsigned long)s->base_color_rgb);
    cJSON_AddStringToObject(obj, "base_color",  hex);

    cJSON_AddNumberToObject(obj, "brightness",  (double)s->brightness);
    cJSON_AddNumberToObject(obj, "led_count",   (double)s->led_count);
    cJSON_AddStringToObject(obj, "auth_mode",
                            s->auth_mode == LL_AUTH_PAIRED ? "paired" : "open");
    cJSON_AddBoolToObject  (obj, "telemetry_enabled", s->telemetry_enabled);

    /* Identity — id is read-only (MAC suffix, cached at boot); name is
     * user-settable via set_state. Clients display name when non-empty,
     * fall back to id for unnamed devices. */
    cJSON_AddStringToObject(obj, "id",   ll_device_id_get());
    cJSON_AddStringToObject(obj, "name", s->name);

    /* Wifi/provisioning state — derived from core/provisioning/, not
     * stored in ll_state_t (the link state isn't a user setting). The
     * webapp uses provisioning_active to decide between #/setup and
     * #/control on load; wifi_ssid populates the settings page. */
    cJSON_AddBoolToObject(obj, "provisioning_active",
                          ll_provisioning_is_active());
    const char *ssid = ll_provisioning_get_sta_ssid();
    if (ssid && ssid[0]) {
        cJSON_AddStringToObject(obj, "wifi_ssid", ssid);
    } else {
        cJSON_AddNullToObject(obj, "wifi_ssid");
    }
    const char *ip = ll_provisioning_get_sta_ip();
    if (ip && ip[0]) {
        cJSON_AddStringToObject(obj, "ip", ip);
    } else {
        cJSON_AddNullToObject(obj, "ip");
    }

    /* Per multi-network-design.md §7.5: count only — full list is
     * fetched on demand via list_wifi_networks. Keeps the broadcast
     * envelope small for the common case (settings page closed). */
    cJSON_AddNumberToObject(obj, "wifi_saved_count",
                            (double)ll_wifi_count());
    return obj;
}

/* ---- payload helpers --------------------------------------------- */

/* Parse "#RRGGBB" or "RRGGBB" into a 0xRRGGBB uint32. Returns false
 * on any malformation (wrong length, non-hex chars, NULL, etc.) so
 * callers can reject quietly rather than commit a bad color. */
static bool parse_hex_color(const char *s, uint32_t *out)
{
    if (!s) return false;
    if (s[0] == '#') s++;
    if (strlen(s) != 6) return false;

    char *end;
    unsigned long v = strtoul(s, &end, 16);
    if (*end != '\0') return false;

    *out = (uint32_t)v & 0x00FFFFFFu;
    return true;
}

/* ---- op handlers ------------------------------------------------- */

static cJSON *result_for_ping(void)
{
    cJSON *result = cJSON_CreateObject();
    if (!result) return NULL;

    const esp_app_desc_t *desc = esp_app_get_description();
    cJSON_AddStringToObject(result, "fw_version", desc->version);
    /* esp_timer_get_time returns microseconds since boot. */
    cJSON_AddNumberToObject(result, "uptime_s",
                            (double)(esp_timer_get_time() / 1000000));
    return result;
}

/* set_state: partial state update. For each recognized field present
 * in the payload, post the matching event to the state-bus loop. The
 * state-bus task applies them sequentially, then re-dispatches
 * LL_EV_STATE_CHANGED — which (Session 2c) transport will broadcast
 * back to all clients. So the response here is a best-effort
 * read-back; the broadcast is authoritative.
 *
 * Sensitive fields are intentionally NOT accepted via set_state:
 *   - auth_mode (use the dedicated set_auth_mode op — security action)
 *   - telemetry_enabled (use set_telemetry — privacy opt-in)
 *   - led_count (read-only, fixed at provisioning / by board header)
 *   - id (read-only MAC suffix, derived at boot, not user-settable)
 *
 * `name` IS accepted via set_state — user-settable, cleared by factory
 * reset since it's part of ll_state_t and lives in the NVS settings blob.
 */
static void op_set_state(cJSON *resp, const cJSON *payload)
{
    if (!cJSON_IsObject(payload)) {
        cJSON_AddBoolToObject(resp, "ok", false);
        cJSON_AddNullToObject(resp, "result");
        cJSON *err = cJSON_AddObjectToObject(resp, "error");
        cJSON_AddStringToObject(err, "code",    "bad_payload");
        cJSON_AddStringToObject(err, "message", "set_state requires an object payload");
        return;
    }

    cJSON *item;

    item = cJSON_GetObjectItem(payload, "on");
    if (cJSON_IsBool(item)) {
        ll_ev_power_toggle_payload_t p = { .on = cJSON_IsTrue(item) };
        ll_state_bus_post(LL_EV_POWER_TOGGLE, &p);
    }

    item = cJSON_GetObjectItem(payload, "base_color");
    {
        const char *s = cJSON_GetStringValue(item);
        uint32_t rgb;
        if (s && parse_hex_color(s, &rgb)) {
            ll_ev_base_color_payload_t p = { .rgb = rgb };
            ll_state_bus_post(LL_EV_BASE_COLOR, &p);
        }
    }

    item = cJSON_GetObjectItem(payload, "brightness");
    if (cJSON_IsNumber(item)) {
        int b = (int)cJSON_GetNumberValue(item);
        if (b >= 0 && b <= 100) {
            ll_ev_brightness_payload_t p = { .value = (uint8_t)b };
            ll_state_bus_post(LL_EV_BRIGHTNESS, &p);
        }
    }

    item = cJSON_GetObjectItem(payload, "pattern_id");
    {
        const char *s = cJSON_GetStringValue(item);
        if (s) {
            ll_ev_pattern_change_payload_t p = {0};
            strncpy(p.id, s, sizeof(p.id) - 1);
            ll_state_bus_post(LL_EV_PATTERN_CHANGE, &p);
        }
    }

    item = cJSON_GetObjectItem(payload, "name");
    {
        const char *s = cJSON_GetStringValue(item);
        if (s) {
            ll_ev_name_change_payload_t p = {0};
            strncpy(p.name, s, sizeof(p.name) - 1);
            ll_state_bus_post(LL_EV_NAME_CHANGE, &p);
        }
    }

    cJSON_AddBoolToObject(resp, "ok", true);
    cJSON_AddItemToObject(resp, "result", state_to_json(ll_state_bus_get()));
    cJSON_AddNullToObject(resp, "error");
}

/* set_wifi_creds: webapp's /setup screen submits {ssid, password} here.
 * We validate, post LL_EV_WIFI_APPLY_CREDS to the state-bus loop, and
 * return ok=true synchronously. The actual SoftAP→STA handoff is done
 * by core/provisioning/'s on_wifi_apply_creds handler — running on a
 * different task than this one, so by the time it executes our JSON
 * response will have flushed and the client will be looking at "socket
 * closed" (its reconnect-with-backoff is already armed for that). */
static void op_set_wifi_creds(cJSON *resp, const cJSON *payload)
{
    if (!cJSON_IsObject(payload)) {
        cJSON_AddBoolToObject(resp, "ok", false);
        cJSON_AddNullToObject(resp, "result");
        cJSON *err = cJSON_AddObjectToObject(resp, "error");
        cJSON_AddStringToObject(err, "code",    "bad_payload");
        cJSON_AddStringToObject(err, "message", "set_wifi_creds requires an object payload");
        return;
    }

    const char *ssid = cJSON_GetStringValue(cJSON_GetObjectItem(payload, "ssid"));
    const char *pw   = cJSON_GetStringValue(cJSON_GetObjectItem(payload, "password"));
    if (!pw) pw = "";

    size_t ssid_len = ssid ? strlen(ssid) : 0;
    size_t pw_len   = strlen(pw);

    /* SSID 1-32 chars (IEEE 802.11). Password either empty (open net)
     * or 8-64 chars (WPA2 spec — though WPA2 actually requires 8-63
     * printable; we accept up to 64 to match the buffer in
     * wifi_config_t.sta.password). */
    if (ssid_len < 1 || ssid_len > 32 ||
        (pw_len != 0 && (pw_len < 8 || pw_len > 64))) {
        cJSON_AddBoolToObject(resp, "ok", false);
        cJSON_AddNullToObject(resp, "result");
        cJSON *err = cJSON_AddObjectToObject(resp, "error");
        cJSON_AddStringToObject(err, "code",    "bad_payload");
        cJSON_AddStringToObject(err, "message",
            "ssid must be 1-32 chars; password must be empty or 8-64 chars");
        return;
    }

    ll_ev_wifi_apply_creds_payload_t p = {0};
    strncpy(p.ssid,     ssid, sizeof(p.ssid) - 1);
    strncpy(p.password, pw,   sizeof(p.password) - 1);
    esp_event_post_to(ll_state_bus_get_loop(), LL_STATE_EVENT_BASE,
                      LL_EV_WIFI_APPLY_CREDS, &p, sizeof(p), 0);

    cJSON_AddBoolToObject(resp, "ok", true);
    cJSON *result = cJSON_AddObjectToObject(resp, "result");
    cJSON_AddBoolToObject(result, "applied", true);
    cJSON_AddStringToObject(result, "ssid", p.ssid);
    cJSON_AddNullToObject(resp, "error");
}

/* ---- multi-network ops (LL-046 step 4 — multi-network-design.md §7) ----
 *
 * list_wifi_networks: read-only enumeration of the saved-networks list.
 * add_wifi_network:    insert-or-update; never switches the active conn.
 * remove_wifi_network: forget by ssid; if removing the active one, the
 *                      mirror disconnects + falls into SCANNING.
 *
 * set_wifi_creds (above) is kept as the legacy single-cred entry point
 * for the first-boot provisioning flow — it writes to ll_wifi via the
 * apply-creds event handler and triggers an immediate SoftAP→STA
 * handoff. The three ops below are the V1+ multi-network surface.
 */

/* selection-sort indices in `out_order` so out_order[0] is the entry
 * with the highest last_used_us. With LL_WIFI_LIST_MAX == 4 the O(n²)
 * cost is trivial and the in-place algorithm avoids any allocation. */
static void sort_indices_by_last_used_desc(const ll_wifi_list_t *list,
                                            uint8_t *out_order)
{
    for (uint8_t i = 0; i < list->count; i++) {
        out_order[i] = i;
    }
    for (uint8_t i = 0; i < list->count; i++) {
        uint8_t best = i;
        for (uint8_t j = i + 1; j < list->count; j++) {
            if (list->entries[out_order[j]].last_used_us
                > list->entries[out_order[best]].last_used_us) {
                best = j;
            }
        }
        if (best != i) {
            uint8_t tmp = out_order[i];
            out_order[i] = out_order[best];
            out_order[best] = tmp;
        }
    }
}

static void op_list_wifi_networks(cJSON *resp)
{
    ll_wifi_list_t snapshot;
    ll_wifi_get_list(&snapshot);

    uint8_t order[LL_WIFI_LIST_MAX];
    sort_indices_by_last_used_desc(&snapshot, order);

    uint8_t active = ll_wifi_get_active_idx();

    cJSON *result = cJSON_CreateObject();
    cJSON *arr = cJSON_AddArrayToObject(result, "networks");
    for (uint8_t k = 0; k < snapshot.count; k++) {
        uint8_t i = order[k];
        cJSON *entry = cJSON_CreateObject();
        cJSON_AddStringToObject(entry, "ssid", snapshot.entries[i].ssid);
        cJSON_AddBoolToObject(entry, "is_active", i == active);
        /* last_used_us is monotonic boot-time microseconds — clients
         * use it for ordering, not as a wall clock. We expose it as
         * seconds to keep the JSON number small. */
        cJSON_AddNumberToObject(entry, "last_used",
                                (double)(snapshot.entries[i].last_used_us / 1000000));
        cJSON_AddItemToArray(arr, entry);
    }

    cJSON_AddBoolToObject(resp, "ok", true);
    cJSON_AddItemToObject(resp, "result", result);
    cJSON_AddNullToObject(resp, "error");
}

static void op_add_wifi_network(cJSON *resp, const cJSON *payload)
{
    if (!cJSON_IsObject(payload)) {
        cJSON_AddBoolToObject(resp, "ok", false);
        cJSON_AddNullToObject(resp, "result");
        cJSON *err = cJSON_AddObjectToObject(resp, "error");
        cJSON_AddStringToObject(err, "code",    "bad_payload");
        cJSON_AddStringToObject(err, "message",
            "add_wifi_network requires an object payload");
        return;
    }

    const char *ssid = cJSON_GetStringValue(cJSON_GetObjectItem(payload, "ssid"));
    const char *pw   = cJSON_GetStringValue(cJSON_GetObjectItem(payload, "password"));
    if (!pw) pw = "";

    size_t ssid_len = ssid ? strlen(ssid) : 0;
    size_t pw_len   = strlen(pw);

    /* Same validation surface as set_wifi_creds — IEEE 802.11 + WPA2. */
    if (ssid_len < 1 || ssid_len > 32 ||
        (pw_len != 0 && (pw_len < 8 || pw_len > 64))) {
        cJSON_AddBoolToObject(resp, "ok", false);
        cJSON_AddNullToObject(resp, "result");
        cJSON *err = cJSON_AddObjectToObject(resp, "error");
        cJSON_AddStringToObject(err, "code",    "bad_payload");
        cJSON_AddStringToObject(err, "message",
            "ssid must be 1-32 chars; password must be empty or 8-64 chars");
        return;
    }

    ll_wifi_add_result_t r = ll_wifi_add(ssid, pw);
    switch (r) {
    case LL_WIFI_ADD_OK_INSERTED:
    case LL_WIFI_ADD_OK_UPDATED: {
        cJSON_AddBoolToObject(resp, "ok", true);
        cJSON *result = cJSON_AddObjectToObject(resp, "result");
        cJSON_AddStringToObject(result, "ssid", ssid);
        cJSON_AddBoolToObject(result, "inserted",
                              r == LL_WIFI_ADD_OK_INSERTED);
        cJSON_AddBoolToObject(result, "updated",
                              r == LL_WIFI_ADD_OK_UPDATED);
        cJSON_AddNullToObject(resp, "error");
        /* Count changed (insert) or list state changed (update — same
         * count but the new password may matter for clients displaying
         * the list); broadcast either way so settings pages refresh. */
        broadcast_state();
        return;
    }
    case LL_WIFI_ADD_ERR_FULL: {
        cJSON_AddBoolToObject(resp, "ok", false);
        cJSON_AddNullToObject(resp, "result");
        cJSON *err = cJSON_AddObjectToObject(resp, "error");
        cJSON_AddStringToObject(err, "code",    "wifi_list_full");
        cJSON_AddStringToObject(err, "message",
            "saved-networks list is full; remove one before adding another");
        return;
    }
    case LL_WIFI_ADD_ERR_INVALID:
    default:
        cJSON_AddBoolToObject(resp, "ok", false);
        cJSON_AddNullToObject(resp, "result");
        cJSON *err = cJSON_AddObjectToObject(resp, "error");
        cJSON_AddStringToObject(err, "code",    "bad_payload");
        cJSON_AddStringToObject(err, "message",
            "ll_wifi_add rejected the entry");
        return;
    }
}

static void op_remove_wifi_network(cJSON *resp, const cJSON *payload)
{
    if (!cJSON_IsObject(payload)) {
        cJSON_AddBoolToObject(resp, "ok", false);
        cJSON_AddNullToObject(resp, "result");
        cJSON *err = cJSON_AddObjectToObject(resp, "error");
        cJSON_AddStringToObject(err, "code",    "bad_payload");
        cJSON_AddStringToObject(err, "message",
            "remove_wifi_network requires an object payload");
        return;
    }

    const char *ssid = cJSON_GetStringValue(cJSON_GetObjectItem(payload, "ssid"));
    if (!ssid || !ssid[0]) {
        cJSON_AddBoolToObject(resp, "ok", false);
        cJSON_AddNullToObject(resp, "result");
        cJSON *err = cJSON_AddObjectToObject(resp, "error");
        cJSON_AddStringToObject(err, "code",    "bad_payload");
        cJSON_AddStringToObject(err, "message",
            "remove_wifi_network requires a non-empty `ssid`");
        return;
    }

    /* Snapshot whether this is the network we're currently on, before
     * the remove mutates active_idx. The app gates this op behind a
     * confirm dialog when target == current SSID (per design-doc §4.3),
     * but we still verify server-side to drive the disconnect. */
    const char *active_ssid = ll_provisioning_get_sta_ssid();
    bool was_active = active_ssid && active_ssid[0]
                       && strcmp(active_ssid, ssid) == 0;

    ll_wifi_remove_result_t r = ll_wifi_remove(ssid);
    if (r == LL_WIFI_REMOVE_NOT_FOUND) {
        cJSON_AddBoolToObject(resp, "ok", false);
        cJSON_AddNullToObject(resp, "result");
        cJSON *err = cJSON_AddObjectToObject(resp, "error");
        cJSON_AddStringToObject(err, "code",    "not_found");
        cJSON_AddStringToObject(err, "message",
            "no saved network with that ssid");
        return;
    }

    if (was_active) {
        /* Proactive disconnect: design-doc §4.3 explicitly diverges from
         * iOS's "stay connected until natural disconnect" pattern.
         * provisioning's STA_DISCONNECTED handler will see SM == ONLINE
         * and transition into SCANNING, picking the next eligible saved
         * network (or BACKOFF if none are visible). */
        ll_provisioning_drop_active();
    }

    cJSON_AddBoolToObject(resp, "ok", true);
    cJSON *result = cJSON_AddObjectToObject(resp, "result");
    cJSON_AddStringToObject(result, "ssid", ssid);
    cJSON_AddBoolToObject(result, "removed",     true);
    cJSON_AddBoolToObject(result, "was_active",  was_active);
    cJSON_AddNullToObject(resp, "error");
    /* wifi_saved_count changed; refresh state on connected clients. */
    broadcast_state();
}

/* connect_wifi_network: switch the active STA to a user-chosen saved
 * network. Bumps the chosen entry's last_used_us so the SM's
 * "highest last_used wins" pick logic favors it on the next scan, then
 * forces a fresh scan-and-pick cycle (regardless of current SM state).
 *
 * Useful when the device is currently on a different saved network,
 * stuck in BACKOFF after a bad-cred attempt, or the user explicitly
 * wants to switch (multi-location use). Returns synchronously — actual
 * connect is async; clients observe via the next state broadcast. If
 * the chosen network isn't visible / has bad creds, the SM falls back
 * to the next eligible saved network (or BACKOFF if none).
 */
static void op_connect_wifi_network(cJSON *resp, const cJSON *payload)
{
    if (!cJSON_IsObject(payload)) {
        cJSON_AddBoolToObject(resp, "ok", false);
        cJSON_AddNullToObject(resp, "result");
        cJSON *err = cJSON_AddObjectToObject(resp, "error");
        cJSON_AddStringToObject(err, "code",    "bad_payload");
        cJSON_AddStringToObject(err, "message",
            "connect_wifi_network requires an object payload");
        return;
    }
    const char *ssid = cJSON_GetStringValue(cJSON_GetObjectItem(payload, "ssid"));
    if (!ssid || !ssid[0]) {
        cJSON_AddBoolToObject(resp, "ok", false);
        cJSON_AddNullToObject(resp, "result");
        cJSON *err = cJSON_AddObjectToObject(resp, "error");
        cJSON_AddStringToObject(err, "code",    "bad_payload");
        cJSON_AddStringToObject(err, "message",
            "connect_wifi_network requires a non-empty `ssid`");
        return;
    }

    if (!ll_wifi_bump_priority(ssid, esp_timer_get_time())) {
        cJSON_AddBoolToObject(resp, "ok", false);
        cJSON_AddNullToObject(resp, "result");
        cJSON *err = cJSON_AddObjectToObject(resp, "error");
        cJSON_AddStringToObject(err, "code",    "not_found");
        cJSON_AddStringToObject(err, "message",
            "no saved network with that ssid");
        return;
    }

    /* Async hop: posting LL_EV_WIFI_REQUEST_SWITCH lets this op finish
     * serializing + sending the response (and the broadcast below)
     * before provisioning's handler calls esp_wifi_disconnect on the
     * state-bus task. Without this, the disconnect kills the WS
     * connection mid-response and the client sees a timeout instead
     * of "switching: true". Same pattern as set_wifi_creds. */
    esp_event_post_to(ll_state_bus_get_loop(), LL_STATE_EVENT_BASE,
                      LL_EV_WIFI_REQUEST_SWITCH, NULL, 0, 0);

    cJSON_AddBoolToObject(resp, "ok", true);
    cJSON *result = cJSON_AddObjectToObject(resp, "result");
    cJSON_AddStringToObject(result, "ssid", ssid);
    cJSON_AddBoolToObject(result, "switching", true);
    cJSON_AddNullToObject(resp, "error");
    /* The list ordering changed (last_used_us updated); refresh
     * settings pages on connected clients before the STA tear-down. */
    broadcast_state();
}

/* factory_reset: replaces the UI mockup from sub-1 with a real wire op.
 * Posts LL_EV_FACTORY_RESET via ll_state_bus_post; downstream handlers
 * (state_bus → reset settings, nvs → erase namespace, provisioning →
 * wipe wifi creds + esp_wifi_restore) take it from there. The device
 * doesn't reboot — it just falls back into the no-creds boot path's
 * runtime state. */
static void op_factory_reset(cJSON *resp)
{
    ll_state_bus_post(LL_EV_FACTORY_RESET, NULL);

    cJSON_AddBoolToObject(resp, "ok", true);
    cJSON *result = cJSON_AddObjectToObject(resp, "result");
    cJSON_AddBoolToObject(result, "reset", true);
    cJSON_AddNullToObject(resp, "error");
}

/* start_ota: pull a binary from `payload.url` and apply as the next-boot
 * firmware. On success the device reboots before this handler returns,
 * so the response we build here is discarded — the client sees the
 * socket close, reconnects, and observes the new fw_version via ping.
 * On failure we fall through and report ota_failed with the underlying
 * esp_err_t name so the client can show something specific. */
static void op_start_ota(cJSON *resp, const cJSON *payload)
{
    if (!cJSON_IsObject(payload)) {
        cJSON_AddBoolToObject(resp, "ok", false);
        cJSON_AddNullToObject(resp, "result");
        cJSON *err = cJSON_AddObjectToObject(resp, "error");
        cJSON_AddStringToObject(err, "code",    "bad_payload");
        cJSON_AddStringToObject(err, "message", "start_ota requires an object payload");
        return;
    }

    const char *url = cJSON_GetStringValue(cJSON_GetObjectItem(payload, "url"));
    if (!url || !url[0]) {
        cJSON_AddBoolToObject(resp, "ok", false);
        cJSON_AddNullToObject(resp, "result");
        cJSON *err = cJSON_AddObjectToObject(resp, "error");
        cJSON_AddStringToObject(err, "code",    "bad_payload");
        cJSON_AddStringToObject(err, "message", "start_ota requires a non-empty `url` string");
        return;
    }

    esp_err_t r = ll_ota_start(url);
    /* Only reachable on failure — success path reboots inside ll_ota_start. */
    cJSON_AddBoolToObject(resp, "ok", false);
    cJSON_AddNullToObject(resp, "result");
    cJSON *err = cJSON_AddObjectToObject(resp, "error");
    cJSON_AddStringToObject(err, "code",    "ota_failed");
    cJSON_AddStringToObject(err, "message", esp_err_to_name(r));
}

/* ---- /api/info HTTP endpoint ------------------------------------- */

/* Single-roundtrip discovery payload. The RN app's findMirrors scans the
 * /24 and probes each IP with GET /api/info — devices respond with a tiny
 * JSON shape that's both a "yes, I'm a Layered Logic mirror" sentinel and
 * the id+name pair the picker needs to label entries. Cheaper than opening
 * a WebSocket per candidate just to call get_state. */
static esp_err_t info_handler(httpd_req_t *req)
{
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return ESP_ERR_NO_MEM;

    const ll_state_t *s = ll_state_bus_get();
    const esp_app_desc_t *desc = esp_app_get_description();

    cJSON_AddStringToObject(obj, "product",    "layered-logic-mirror");
    cJSON_AddStringToObject(obj, "id",         ll_device_id_get());
    cJSON_AddStringToObject(obj, "name",       s->name);
    cJSON_AddStringToObject(obj, "fw_version", desc->version);

    char *out = cJSON_PrintUnformatted(obj);
    cJSON_Delete(obj);
    if (!out) return ESP_ERR_NO_MEM;

    httpd_resp_set_type(req, "application/json; charset=utf-8");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    /* Allow cross-origin probes from a future web-based discovery page —
     * the response carries no secrets, just the public identity already
     * advertised over mDNS. */
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    esp_err_t err = httpd_resp_send(req, out, strlen(out));
    free(out);
    return err;
}

static const httpd_uri_t URI_INFO = {
    .uri      = "/api/info",
    .method   = HTTP_GET,
    .handler  = info_handler,
    .user_ctx = NULL,
};

/* ---- envelope dispatch ------------------------------------------- */

static esp_err_t send_text_frame(httpd_req_t *req, const char *json, size_t len)
{
    httpd_ws_frame_t frame = {
        .final = true,
        .fragmented = false,
        .type = HTTPD_WS_TYPE_TEXT,
        .payload = (uint8_t *)json,
        .len = len,
    };
    return httpd_ws_send_frame(req, &frame);
}

static esp_err_t handle_envelope(httpd_req_t *req, const char *json)
{
    cJSON *envelope = cJSON_Parse(json);
    if (!envelope) {
        ESP_LOGW(TAG, "bad JSON envelope — dropping frame");
        return ESP_OK;  /* keep connection alive; client gets nothing */
    }

    const char *op     = cJSON_GetStringValue(cJSON_GetObjectItem(envelope, "op"));
    const char *req_id = cJSON_GetStringValue(cJSON_GetObjectItem(envelope, "req_id"));

    cJSON *resp = cJSON_CreateObject();
    if (!resp) {
        cJSON_Delete(envelope);
        return ESP_ERR_NO_MEM;
    }

    cJSON_AddStringToObject(resp, "op",     op     ? op     : "");
    cJSON_AddStringToObject(resp, "req_id", req_id ? req_id : "");

    if (op && strcmp(op, "ping") == 0) {
        cJSON_AddBoolToObject(resp, "ok", true);
        cJSON_AddItemToObject(resp, "result", result_for_ping());
        cJSON_AddNullToObject(resp, "error");
    } else if (op && strcmp(op, "get_state") == 0) {
        cJSON_AddBoolToObject(resp, "ok", true);
        cJSON_AddItemToObject(resp, "result", state_to_json(ll_state_bus_get()));
        cJSON_AddNullToObject(resp, "error");
    } else if (op && strcmp(op, "set_state") == 0) {
        op_set_state(resp, cJSON_GetObjectItem(envelope, "payload"));
    } else if (op && strcmp(op, "set_wifi_creds") == 0) {
        op_set_wifi_creds(resp, cJSON_GetObjectItem(envelope, "payload"));
    } else if (op && strcmp(op, "list_wifi_networks") == 0) {
        op_list_wifi_networks(resp);
    } else if (op && strcmp(op, "add_wifi_network") == 0) {
        op_add_wifi_network(resp, cJSON_GetObjectItem(envelope, "payload"));
    } else if (op && strcmp(op, "remove_wifi_network") == 0) {
        op_remove_wifi_network(resp, cJSON_GetObjectItem(envelope, "payload"));
    } else if (op && strcmp(op, "connect_wifi_network") == 0) {
        op_connect_wifi_network(resp, cJSON_GetObjectItem(envelope, "payload"));
    } else if (op && strcmp(op, "factory_reset") == 0) {
        op_factory_reset(resp);
    } else if (op && strcmp(op, "start_ota") == 0) {
        op_start_ota(resp, cJSON_GetObjectItem(envelope, "payload"));
    } else {
        cJSON_AddBoolToObject(resp, "ok", false);
        cJSON_AddNullToObject(resp, "result");
        cJSON *err = cJSON_AddObjectToObject(resp, "error");
        cJSON_AddStringToObject(err, "code", "unknown_op");
        cJSON_AddStringToObject(err, "message",
            op ? "Op not implemented in this firmware version"
               : "Missing op field in envelope");
    }

    char *out = cJSON_PrintUnformatted(resp);
    cJSON_Delete(resp);
    cJSON_Delete(envelope);

    if (!out) return ESP_ERR_NO_MEM;

    esp_err_t err = send_text_frame(req, out, strlen(out));
    free(out);

    if (err != ESP_OK) {
        ESP_LOGW(TAG, "ws send: %s", esp_err_to_name(err));
    }
    return err;
}

/* ---- WS URI handler ---------------------------------------------- */

static esp_err_t ws_handler(httpd_req_t *req)
{
    /* esp_http_server invokes the registered URI handler twice per
     * connection: once for the upgrade handshake (method=HTTP_GET)
     * and again for each subsequent frame (other methods). */
    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "ws handshake from sock=%d", httpd_req_to_sockfd(req));
        return ESP_OK;
    }

    httpd_ws_frame_t frame = {0};
    frame.type = HTTPD_WS_TYPE_TEXT;

    /* Two-phase recv: max_len=0 fills frame.len without copying. */
    esp_err_t err = httpd_ws_recv_frame(req, &frame, 0);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "ws recv (size): %s", esp_err_to_name(err));
        return err;
    }

    if (frame.len == 0) {
        return ESP_OK;
    }
    if (frame.len > LL_WS_RX_MAX) {
        ESP_LOGW(TAG, "ws frame too large: %u bytes (max %d)",
                 (unsigned)frame.len, LL_WS_RX_MAX);
        return ESP_FAIL;
    }

    uint8_t *buf = calloc(1, frame.len + 1);  /* +1 for cJSON null term */
    if (!buf) return ESP_ERR_NO_MEM;
    frame.payload = buf;

    err = httpd_ws_recv_frame(req, &frame, frame.len);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "ws recv (payload): %s", esp_err_to_name(err));
        free(buf);
        return err;
    }

    if (frame.type == HTTPD_WS_TYPE_TEXT) {
        err = handle_envelope(req, (const char *)buf);
    }
    /* Binary / ping / pong frames: ignored at the app layer for
     * Session 1. The httpd layer handles WS protocol-level ping/pong
     * automatically when CONFIG_HTTPD_WS_SUPPORT=y. */

    free(buf);
    return err;
}

/* ---- lifecycle --------------------------------------------------- */

static const httpd_uri_t URI_WS = {
    .uri          = "/ws",
    .method       = HTTP_GET,
    .handler      = ws_handler,
    .user_ctx     = NULL,
    .is_websocket = true,
};

static esp_err_t start_server(void)
{
    if (g_server) return ESP_OK;

    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    cfg.server_port      = LL_TRANSPORT_PORT;
    cfg.max_uri_handlers = 8;     /* /ws + /api/info + 3 webapp assets + headroom for captive-portal probe URIs */
    cfg.lru_purge_enable = true;  /* close oldest socket on overflow */

    esp_err_t err = httpd_start(&g_server, &cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "httpd_start: %s", esp_err_to_name(err));
        g_server = NULL;
        return err;
    }

    err = httpd_register_uri_handler(g_server, &URI_WS);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "register %s: %s", URI_WS.uri, esp_err_to_name(err));
        httpd_stop(g_server);
        g_server = NULL;
        return err;
    }

    /* /api/info before webapp_assets so it wins URI matching against the
     * 404→/ captive-portal redirect that webapp_assets installs. */
    err = httpd_register_uri_handler(g_server, &URI_INFO);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "register %s: %s", URI_INFO.uri, esp_err_to_name(err));
        httpd_stop(g_server);
        g_server = NULL;
        return err;
    }

    err = ll_webapp_assets_register(g_server);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "webapp assets register: %s", esp_err_to_name(err));
        httpd_stop(g_server);
        g_server = NULL;
        return err;
    }

    ESP_LOGI(TAG, "listening on :%d (ws %s, info %s)",
             LL_TRANSPORT_PORT, URI_WS.uri, URI_INFO.uri);
    return ESP_OK;
}

static void stop_server(void)
{
    if (!g_server) return;
    httpd_stop(g_server);
    g_server = NULL;
    ESP_LOGI(TAG, "stopped");
}

static void on_wifi_connected(void *arg, esp_event_base_t base,
                              int32_t id, void *data)
{
    (void)arg; (void)base; (void)id; (void)data;
    start_server();
    /* The provisioning_active and wifi_ssid wire fields just changed —
     * push a state broadcast so any client that's already connected
     * (e.g. the webapp on the SoftAP, before we transition to STA, or
     * a future browser that reconnected after STA came up) sees the
     * fresh values. broadcast_state() is a no-op when no clients are
     * connected. */
    broadcast_state();
}

static void on_wifi_disconnected(void *arg, esp_event_base_t base,
                                 int32_t id, void *data)
{
    (void)arg; (void)base; (void)id; (void)data;
    /* Broadcast BEFORE stop_server so any connected clients see the
     * cleared wifi_ssid before their socket dies. Realistically the
     * frame won't make it out (the wifi link is gone), but the call
     * is cheap and keeps the lifecycle ordering honest. */
    broadcast_state();
    stop_server();
}

/* ---- state broadcast --------------------------------------------- */

/* Schedule a state broadcast to all connected WS clients. Coalesces
 * rapid-fire callers within LL_BROADCAST_DEBOUNCE_US — the first call
 * arms the timer; subsequent calls inside the window return
 * immediately. The actual broadcast (do_broadcast_state) runs on the
 * esp_timer task and captures state at fire-time, so intermediate
 * states from a multi-field set_state burst are dropped and only the
 * settled state ships to clients.
 *
 * Thread-safe across the state-bus, httpd, and esp_timer tasks via
 * stdatomic.h. Called from on_state_changed (state-bus task),
 * on_wifi_connected/disconnected (state-bus task), and the multi-network
 * wifi op handlers (httpd task).
 */
static void broadcast_state(void)
{
    bool was_pending = atomic_exchange(&s_broadcast_pending, true);
    if (was_pending) return;  /* one already queued — coalesce this one in */

    esp_err_t err = esp_timer_start_once(s_broadcast_timer,
                                          LL_BROADCAST_DEBOUNCE_US);
    if (err != ESP_OK) {
        /* Timer wedged for some reason — fall back to immediate broadcast
         * so we don't silently drop state updates. */
        atomic_store(&s_broadcast_pending, false);
        ESP_LOGW(TAG, "broadcast timer start: %s — falling back to immediate",
                 esp_err_to_name(err));
        do_broadcast_state();
    }
}

static void broadcast_timer_cb(void *arg)
{
    (void)arg;
    atomic_store(&s_broadcast_pending, false);
    do_broadcast_state();
}

/* Actually serialize + push the broadcast envelope to all connected WS
 * clients. Runs on the esp_timer task; should never be called directly
 * — go through broadcast_state() so the debounce holds. */
static void do_broadcast_state(void)
{
    if (!g_server) return;

    cJSON *env = cJSON_CreateObject();
    if (!env) return;
    cJSON_AddStringToObject(env, "op", "state");
    cJSON_AddNumberToObject(env, "ts",
                            (double)(esp_timer_get_time() / 1000000));
    cJSON_AddItemToObject  (env, "state", state_to_json(ll_state_bus_get()));

    char *json = cJSON_PrintUnformatted(env);
    cJSON_Delete(env);
    if (!json) return;

    /* Enumerate active sockets, filter to WebSocket clients, send. */
    int fds[16];
    size_t n = sizeof(fds) / sizeof(fds[0]);
    if (httpd_get_client_list(g_server, &n, fds) != ESP_OK) {
        free(json);
        return;
    }

    httpd_ws_frame_t frame = {
        .final      = true,
        .fragmented = false,
        .type       = HTTPD_WS_TYPE_TEXT,
        .payload    = (uint8_t *)json,
        .len        = strlen(json),
    };

    int delivered = 0;
    for (size_t i = 0; i < n; i++) {
        if (httpd_ws_get_fd_info(g_server, fds[i]) != HTTPD_WS_CLIENT_WEBSOCKET) {
            continue;
        }
        esp_err_t err = httpd_ws_send_frame_async(g_server, fds[i], &frame);
        if (err == ESP_OK) {
            delivered++;
        } else {
            ESP_LOGW(TAG, "broadcast to fd=%d: %s", fds[i], esp_err_to_name(err));
            /* Don't bail — try other clients. Framework reaps dead
             * sockets on its own; we'll skip them next round. */
        }
    }

    if (delivered > 0) {
        ESP_LOGI(TAG, "broadcast state to %d ws client(s) (%u bytes)",
                 delivered, (unsigned)frame.len);
    }

    free(json);
}

static void on_state_changed(void *arg, esp_event_base_t base,
                             int32_t id, void *data)
{
    (void)arg; (void)base; (void)id; (void)data;
    broadcast_state();
}

/* ---- public entry points ----------------------------------------- */

esp_err_t ll_transport_init(void)
{
    /* esp_http_server is started lazily on WIFI_CONNECTED. The broadcast
     * debounce timer is created here so it's ready before the first
     * LL_EV_STATE_CHANGED can fire — it's a one-shot timer that
     * broadcast_state() arms on demand. */
    const esp_timer_create_args_t args = {
        .callback = broadcast_timer_cb,
        .name     = "ll_broadcast",
    };
    esp_err_t err = esp_timer_create(&args, &s_broadcast_timer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_timer_create: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "init: server deferred until wifi up");
    return ESP_OK;
}

esp_err_t ll_transport_subscribe(void)
{
    esp_err_t err = esp_event_handler_instance_register_with(
        ll_state_bus_get_loop(),
        LL_STATE_EVENT_BASE, LL_EV_WIFI_CONNECTED,
        on_wifi_connected, NULL, NULL);
    if (err != ESP_OK) return err;

    err = esp_event_handler_instance_register_with(
        ll_state_bus_get_loop(),
        LL_STATE_EVENT_BASE, LL_EV_WIFI_DISCONNECTED,
        on_wifi_disconnected, NULL, NULL);
    if (err != ESP_OK) return err;

    return esp_event_handler_instance_register_with(
        ll_state_bus_get_loop(),
        LL_STATE_EVENT_BASE, LL_EV_STATE_CHANGED,
        on_state_changed, NULL, NULL);
}
