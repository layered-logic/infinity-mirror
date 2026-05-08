#pragma once

/*
 * Wi-Fi provisioning + station lifecycle.
 *
 * Responsibility (firmware-spec §4.4):
 *   - Boot: if NVS has saved Wi-Fi creds → bring up esp_wifi station and
 *     connect. If not → leave radios dark. Provisioning is opt-in, gated
 *     by a deliberate user action (recessed-button short-hold).
 *   - On LL_EV_PROVISION_START: run wifi_prov_mgr for up to 5 minutes
 *     (SoftAP scheme for now; BLE deferred — see comment in provisioning.c).
 *     On successful cred receipt, wifi_prov_mgr saves them in the
 *     esp_wifi NVS namespace and the station connect kicks in.
 *   - On LL_EV_FACTORY_RESET: wipe Wi-Fi creds via esp_wifi_restore().
 *
 * Posts on the ll state-bus loop:
 *   - LL_EV_WIFI_CONNECTED       when station gets an IP
 *   - LL_EV_WIFI_DISCONNECTED    when station drops (after retries)
 *
 * Ownership boundary: this module owns the esp_wifi / wifi_prov_mgr NVS
 * namespaces. core/nvs/ owns ll_settings and must not touch them.
 *
 * Wiring order in main():
 *     ll_nvs_init → ll_state_bus_init → ll_nvs_subscribe
 *                                     → ll_provisioning_init
 *                                     → ll_provisioning_subscribe
 */

#include <stdbool.h>

#include "esp_err.h"

/*
 * Initialize netif, default event loop, esp_wifi, and wifi_prov_mgr.
 * If creds are already saved, brings up the station and triggers a
 * connect attempt. Safe to call exactly once at boot.
 */
esp_err_t ll_provisioning_init(void);

/*
 * Subscribe to LL_EV_PROVISION_START and LL_EV_FACTORY_RESET on the
 * state-bus loop. Call after ll_state_bus_init() and after
 * ll_provisioning_init().
 */
esp_err_t ll_provisioning_subscribe(void);

/*
 * True while the device is in SoftAP mode awaiting credentials — covers
 * both the dev open-SoftAP path and the wifi_prov_mgr-driven path. The
 * webapp uses this (via the wire state object) to decide whether to
 * show the /setup screen or the /control screen on load.
 */
bool ll_provisioning_is_active(void);

/*
 * Current STA SSID (the network the device is connected to as a client),
 * or an empty string when the device is not on a STA network. Backed by
 * a static buffer; valid until the next WIFI_CONNECTED / DISCONNECTED
 * transition. Caller does not free.
 */
const char *ll_provisioning_get_sta_ssid(void);

/*
 * Current STA IPv4 address as a dotted-quad string ("192.168.5.229"), or
 * an empty string when the device is not on a STA network. Same lifetime
 * + ownership rules as ll_provisioning_get_sta_ssid.
 */
const char *ll_provisioning_get_sta_ip(void);

/*
 * Disconnect the current STA connection (if any). Used by transport's
 * `remove_wifi_network` op when the user forgets the network the mirror
 * is currently connected to — per multi-network-design.md §4.3, the
 * mirror disconnects immediately rather than the iOS-style "stay
 * connected until natural disconnect" pattern.
 *
 * After the disconnect, the wifi event handler sees SM in ONLINE state,
 * posts LL_EV_WIFI_DISCONNECTED, and transitions the SM back into
 * SCANNING — which will pick the next eligible saved network.
 *
 * No-op when not currently STA-connected. Safe to call from any task.
 */
void ll_provisioning_drop_active(void);

/*
 * Force a fresh scan-and-pick cycle now. Used by transport's
 * `connect_wifi_network` op after bumping a saved entry's priority,
 * so the SM picks the user-chosen network without waiting out the
 * current backoff or letting the in-flight CONNECTING attempt finish.
 *
 * Behavior depends on current SM state:
 *   ONLINE     → disconnect (handler kicks SCANNING)
 *   CONNECTING → mark current attempt failed, disconnect → SCANNING
 *   BACKOFF    → cancel timer, enter SCANNING immediately
 *   SCANNING   → no-op (current scan will produce the right pick)
 *   IDLE       → no-op (no saved networks; caller should have guarded)
 */
void ll_provisioning_request_switch(void);

/*
 * Kick the deferred SoftAP that ll_provisioning_init configured but
 * didn't actually start. Call this from main.c AFTER every *_subscribe
 * has run, so downstream modules (mdns, transport, captive_dns) catch
 * the synthesized LL_EV_WIFI_CONNECTED event the AP_START handler
 * posts. Without the deferred-start pattern, the AP comes up so fast
 * that LL_EV_WIFI_CONNECTED fires before subscribers register and is
 * silently dropped.
 *
 * No-op when LL_SOFTAP_PROVISIONING is 0 (e.g., a future BLE-only
 * variant) or when the device booted with creds saved (STA path took
 * over instead). Safe to call unconditionally from main.c.
 */
esp_err_t ll_provisioning_kick_softap(void);
