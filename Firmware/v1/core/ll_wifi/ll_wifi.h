#pragma once

/*
 * Multi-network Wi-Fi credential store.
 *
 * Owns the `ll_wifi` NVS namespace. Holds up to LL_WIFI_LIST_MAX saved
 * networks (ssid + password + last-used timestamp) per
 * docs/multi-network-design.md §5.
 *
 * This module only manages the *store* — it does not drive the radio.
 * core/provisioning/ reads ll_wifi to pick which network to connect to,
 * and reports back via ll_wifi_set_active() on a successful connect.
 *
 * Wiring order in main():
 *     ll_nvs_init                 → initializes nvs_flash
 *     ll_state_bus_init
 *     ll_nvs_subscribe
 *     ll_provisioning_init        → brings up esp_netif + esp_wifi
 *     ll_wifi_init                → loads namespace; runs migration shim
 *                                   if list is empty and esp_wifi has a
 *                                   saved STA cred
 *     (provisioning's connect logic now reads ll_wifi to pick a network)
 *
 * Concurrency: the public API is meant for low-rate control paths
 * (state-bus loop callbacks, transport request handlers). Internal
 * access is serialized through a mutex.
 */

#include <stdbool.h>

#include "esp_err.h"

#include "ll_wifi_logic.h"

/*
 * Initialize the store. Must be called after nvs_flash_init (i.e., after
 * ll_nvs_init).
 *
 * Loads count, entry_0..entry_{count-1}, and active_idx from the `ll_wifi`
 * namespace. Returns ESP_OK on any non-fatal outcome (including "namespace
 * empty"); propagates an esp_err_t only on hard NVS failures.
 *
 * Does NOT run the migration shim — call ll_wifi_migrate_from_esp_wifi()
 * separately once the radio is up and a saved STA cred is observable.
 */
esp_err_t ll_wifi_init(void);

/*
 * Migration shim from the legacy single-cred state (§5.3).
 *
 * If the list is currently empty AND `ssid` is non-empty, copy the
 * (ssid, password) pair into entry 0 and persist. No-op otherwise.
 *
 * Caller is expected to have queried esp_wifi_get_config(WIFI_IF_STA, ...)
 * to obtain the cred. We don't read esp_wifi here so that the dependency
 * direction stays one-way (provisioning depends on ll_wifi, not the
 * other way around).
 */
esp_err_t ll_wifi_migrate_from_esp_wifi(const char *ssid, const char *password);

/* Snapshot the list. Caller's copy — safe to read without locking. */
void ll_wifi_get_list(ll_wifi_list_t *out);

/* Number of saved networks (0..LL_WIFI_LIST_MAX). */
uint8_t ll_wifi_count(void);

/*
 * Resolve an SSID to a saved entry. Returns true and fills *out if found;
 * false otherwise. Password is included — caller is responsible for not
 * leaking it back over the wire protocol.
 */
bool ll_wifi_get_by_ssid(const char *ssid, ll_wifi_entry_t *out);

/* Index of the last successfully-connected entry, or LL_WIFI_ACTIVE_NONE. */
uint8_t ll_wifi_get_active_idx(void);

/*
 * Insert or replace by SSID. Persists the change to NVS before returning.
 */
ll_wifi_add_result_t ll_wifi_add(const char *ssid, const char *password);

/*
 * Remove by SSID. Persists the change to NVS before returning. Caller is
 * responsible for triggering a disconnect if the removed entry was the
 * active one — this module only manages the store, not the radio.
 */
ll_wifi_remove_result_t ll_wifi_remove(const char *ssid);

/*
 * Mark an entry as the currently-active connection and stamp its
 * last_used_us. Persists both changes.
 *
 * Returns ESP_ERR_INVALID_ARG if idx is out of range or refers to a
 * slot beyond `count`.
 */
esp_err_t ll_wifi_set_active(uint8_t idx, int64_t last_used_us);

/*
 * Clear the active flag (e.g., on STA disconnect). Persists. Does not
 * touch any entry's last_used_us.
 */
void ll_wifi_clear_active(void);

/*
 * Erase the entire ll_wifi namespace. Used on factory reset, before the
 * caller wipes esp_wifi's saved cred.
 */
esp_err_t ll_wifi_erase_all(void);
