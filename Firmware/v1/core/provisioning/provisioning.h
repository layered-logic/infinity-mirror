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
 * Dev-only: kick the deferred SoftAP that ll_provisioning_init configured
 * but didn't actually start. Call this from main.c AFTER every *_subscribe
 * has run, so downstream modules (mdns, transport) catch the synthesized
 * LL_EV_WIFI_CONNECTED event the AP_START handler posts.
 *
 * No-op when LL_DEV_OPEN_SOFTAP is 0 (production builds) or when the
 * device booted with creds saved (STA path took over instead). Safe to
 * call unconditionally from main.c.
 *
 * Replaced by the captive-portal flow in Session 5 of the app demo
 * mini-sprint.
 */
esp_err_t ll_provisioning_kick_dev_softap(void);
