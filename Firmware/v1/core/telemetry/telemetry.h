#pragma once

/*
 * Opt-in telemetry beacons — firmware side.
 *
 * Spec: docs/firmware-spec.md §4.10
 * Tracked by:  LL-057-C (Sprint 8)
 * Endpoint:    https://telemetry.layeredlogic.cc/v1/beacon  (Cloudflare Worker
 *              at cloudflare-workers/telemetry/, schema documented in that
 *              directory's README.md)
 *
 * Cadence: once per 24h ± 2h jitter, starting 1-5 min after the first
 * LL_EV_WIFI_CONNECTED of the boot. The cadence runs unconditionally;
 * each fire checks state.telemetry_enabled and only POSTs when the
 * user has opted in (default false). Skipping a beacon does NOT reset
 * the cadence — we just sleep again.
 *
 * Payload (see Worker's index.ts for full validation):
 *   { v:1, device_id, fw_version, uptime_s, heap_free_min,
 *     boot_reason, rssi, led_count, panic? }
 *
 * Panic info: at init, if esp_reset_reason() == ESP_RST_PANIC we set a
 * "this boot was a panic" flag that's included in the very next beacon,
 * then cleared. V1 only captures the reason; the spec's full
 * filename:line:task_name capture is deferred — needs a custom panic
 * handler hook (esp_set_panic_handler / esp_core_dump) and is bigger
 * than this MVP.
 */

#include "esp_err.h"

/* Create the beacon timer + record this boot's reset reason. Idempotent. */
esp_err_t ll_telemetry_init(void);

/* Subscribe to LL_EV_WIFI_CONNECTED so the cadence starts after the
 * first successful network join. Idempotent. */
esp_err_t ll_telemetry_subscribe(void);
