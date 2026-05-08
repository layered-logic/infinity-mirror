#pragma once

/*
 * OTA — minimal HTTP(S)-pull firmware update, dev-test scope.
 *
 * Production OTA per firmware-architecture-scoping.md §5.4 will use:
 *   - ECDSA P-256 signed binaries (key in offline password manager)
 *   - Cloudflare Worker + R2 hosting at ota.layeredlogic.cc
 *   - Anti-rollback (CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK eFuse)
 *   - Auto-revert on health-check failure (60s window)
 *   - Staged rollout via device-ID hash buckets
 *
 * NONE of that is here. This module is the throwaway dev-test path:
 * unsigned, plain HTTP, no rollback machinery, no health check. It
 * exists to validate that the A/B partition boot ordering and the
 * esp_https_ota client integration both actually work end-to-end on
 * silicon, before we invest in the production plumbing.
 *
 * Safe-to-throw because:
 *   - eFuses are untouched (per the demo-build hardware safety rules)
 *   - factory partition is preserved (only ota_0/ota_1 ever rewritten)
 *   - bad OTA → bootloader falls back to last-known-good slot
 *
 * After production OTA lands, this header gets a public `ll_ota_init`
 * that wires into ota.layeredlogic.cc; the throwaway path can be
 * deleted by ripping this whole module and replacing.
 */

#include "esp_err.h"

/* No upfront state needed; here for symmetry with the rest of core/. */
esp_err_t ll_ota_init(void);

/*
 * Subscribe to LL_EV_WIFI_CONNECTED on the state-bus loop. The first
 * occurrence after boot calls esp_ota_mark_app_valid_cancel_rollback()
 * — telling the bootloader "this image got online, it's healthy."
 *
 * Without this, an OTA-installed image that panics during early boot
 * (or never reaches the network) gets rolled back to the previous
 * slot on the next reset. That's the safety net for dev OTAs that
 * brick the device — see CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE in
 * sdkconfig.defaults.
 *
 * Call after ll_state_bus_init() and ll_ota_init().
 */
esp_err_t ll_ota_subscribe(void);

/*
 * Pull `url` and apply as the next-boot firmware. Synchronous — blocks
 * until the binary is fully downloaded + validated + the active
 * partition is swapped, then calls esp_restart() (so the function does
 * not return on success). On failure returns the underlying error from
 * esp_https_ota and the caller's partition stays untouched.
 *
 *   url   — http://host[:port]/path. https:// works too but the dev-test
 *           path uses plain http to avoid the mbedtls overhead.
 */
esp_err_t ll_ota_start(const char *url);
