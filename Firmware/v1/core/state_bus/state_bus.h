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
