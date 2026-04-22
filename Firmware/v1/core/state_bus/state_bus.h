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

/* Scaffold-only stubs. Real signatures land when the event-loop task
 * implementation is written. */
void ll_state_bus_init(void);
const ll_state_t *ll_state_bus_get(void);
void ll_state_bus_post(ll_event_t ev, const void *payload);
