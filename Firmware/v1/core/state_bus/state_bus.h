#pragma once

/*
 * Unified state bus — single source of truth for device state.
 *
 * All controllers (button, transport, matter bridge) post events here.
 * The state-bus task applies changes and notifies subscribers. No module
 * mutates ll_state_t directly.
 *
 * See firmware-spec.md §3 for the full design.
 */

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    LL_AUTH_OPEN = 0,
    LL_AUTH_PAIRED = 1,
} ll_auth_mode_t;

typedef struct {
    bool on;
    char pattern_id[32];
    uint32_t base_color_rgb;   /* 0xRRGGBB */
    uint8_t brightness;        /* 0-100; actual snap points: 25/50/75/100 */
    uint16_t led_count;
    ll_auth_mode_t auth_mode;
    bool telemetry_enabled;
    char name[33];             /* user-set; empty = "fall back to id" */
} ll_state_t;

typedef enum {
    LL_EV_POWER_TOGGLE,
    LL_EV_BASE_COLOR,
    LL_EV_PATTERN_CHANGE,
    LL_EV_BRIGHTNESS,
    LL_EV_AUTH_MODE,
    LL_EV_TELEMETRY,
    LL_EV_PROVISION_START,
    LL_EV_FACTORY_RESET,
    LL_EV_NAME_CHANGE,
} ll_event_t;

/*
 * Initialize the state-bus task and event loop.
 *
 * If `initial` is non-NULL, its contents are copied into the internal
 * ll_state_t. Pass the value loaded from NVS here. If NULL, compiled
 * defaults from ll_state_defaults() are used — suitable for first boot
 * when NVS isn't available yet, or for unit tests.
 */
void ll_state_bus_init(const ll_state_t *initial);

const ll_state_t *ll_state_bus_get(void);
void ll_state_bus_post(ll_event_t ev, const void *payload);

/*
 * Write the compiled default state into *out. Used by nvs/ on first boot
 * or schema mismatch. Separated from init so the defaults are a single
 * source of truth regardless of how state_bus gets initialized.
 */
void ll_state_defaults(ll_state_t *out);

/*
 * Lower 3 bytes of the WiFi STA MAC, lowercase hex (6 chars + null).
 * Read once at ll_state_bus_init() time. Empty string until init runs.
 *
 * This is the same suffix mDNS publishes in its TXT record and the SoftAP
 * exposes in its SSID — the device's stable hardware identity, distinct
 * from the user-settable ll_state_t.name. Returned as a const pointer to
 * a static buffer; do not free.
 */
const char *ll_device_id_get(void);
