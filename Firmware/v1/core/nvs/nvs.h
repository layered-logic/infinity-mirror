#pragma once

/*
 * NVS-backed settings persistence.
 *
 * Owns the `ll_settings` NVS namespace (user prefs + schema version).
 * Does NOT own Wi-Fi credentials (those live in the esp_wifi / wifi_prov_mgr
 * namespaces managed by core/provisioning/) or pairing secrets (core/auth/).
 *
 * Wiring order in main():
 *     ll_state_t initial;
 *     bool first_boot;
 *     ESP_ERROR_CHECK(ll_nvs_init(&initial, &first_boot));
 *     ll_state_bus_init(&initial);
 *     ESP_ERROR_CHECK(ll_nvs_subscribe());
 *
 * After that, any LL_EV_STATE_CHANGED event triggers a debounced save.
 * LL_EV_FACTORY_RESET erases the namespace instead of saving.
 */

#include <stdbool.h>

#include "esp_err.h"
#include "state_bus.h"

/*
 * Initialize nvs_flash and load persisted settings into *out.
 *
 * On first boot (namespace/key not found) or schema mismatch, *out is
 * populated with compiled defaults (ll_state_defaults) and *first_boot_out
 * is set to true. On a successful load, *first_boot_out is false.
 *
 * Returns ESP_OK on any non-fatal outcome (including "used defaults
 * because flash was empty"). Returns a propagated esp_err_t only when
 * nvs_flash_init itself fails in a way we can't recover from.
 */
esp_err_t ll_nvs_init(ll_state_t *out, bool *first_boot_out);

/*
 * Subscribe to LL_EV_STATE_CHANGED (debounced save) and to the factory-
 * reset path on the state-bus loop. Call after ll_state_bus_init().
 */
esp_err_t ll_nvs_subscribe(void);

/*
 * Force any pending debounced save to flash immediately. Useful before a
 * voluntary reboot or OTA apply. Safe to call when no save is pending.
 */
void ll_nvs_flush(void);
