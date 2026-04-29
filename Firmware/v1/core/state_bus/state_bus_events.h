#pragma once

/*
 * Event base, payload structs, and loop accessor for state_bus.
 *
 * Kept separate from state_bus.h so the public client API (init/get/post)
 * stays a small 3-function surface. Anything that *subscribes* to state
 * changes needs this header; anything that only *posts* can get away with
 * just state_bus.h (payload structs aside).
 */

#include <stdbool.h>
#include <stdint.h>

#include "esp_event.h"

#include "state_bus.h"

ESP_EVENT_DECLARE_BASE(LL_STATE_EVENT_BASE);

/*
 * Notification event posted by the state-bus task after an incoming
 * LL_EV_* has been applied to ll_state_t. Subscribers (pattern_interp,
 * transport, nvs, matter_bridge) register on LL_EV_STATE_CHANGED and call
 * ll_state_bus_get() to read the new state. The `which` field lets them
 * skip work they don't care about without blindly reacting to every post.
 *
 * Numeric id chosen well outside the ll_event_t enum range so the internal
 * handler can distinguish incoming posts from its own re-dispatches.
 */
#define LL_EV_STATE_CHANGED   0x1000

/*
 * Transient recessed-button hold events. Posted by the button module on
 * raw press/release of the recessed pin (not on gesture resolution), so
 * subscribers can show in-flight feedback while the user is still deciding
 * how long to hold. These don't mutate ll_state_t — pattern_interp is the
 * primary consumer (blue-blink cue).
 *
 * No payload; carried as id-only posts on LL_STATE_EVENT_BASE. Begin is
 * idempotent per press; End always follows Begin (or never, if release
 * came before the debounce window cleared).
 */
#define LL_EV_RECESSED_HOLD_BEGIN   0x1001
#define LL_EV_RECESSED_HOLD_END     0x1002

/*
 * Wi-Fi link-state transitions, posted by core/provisioning/ after the
 * underlying IDF WIFI_EVENT / IP_EVENT fires. Fan-out events — don't
 * mutate ll_state_t (Wi-Fi link isn't user settings), but mdns, transport,
 * telemetry, and ota care. Coalesced: rapid disconnect/reconnect storms
 * post at most one CONNECTED per actual IP acquisition.
 */
#define LL_EV_WIFI_CONNECTED        0x1003
#define LL_EV_WIFI_DISCONNECTED     0x1004

/*
 * Apply Wi-Fi STA credentials and transition out of SoftAP. Posted by
 * core/transport/ on receipt of a `set_wifi_creds` JSON op; consumed
 * by core/provisioning/ which performs the actual SoftAP→STA handoff
 * asynchronously (so the JSON response can flush before the underlying
 * socket dies). Carries the credentials by value; no caller liveness
 * required after the post.
 */
#define LL_EV_WIFI_APPLY_CREDS      0x1005

/*
 * Payloads for each incoming LL_EV_*. esp_event copies these by value at
 * post time, so callers may pass stack pointers freely. String fields use
 * fixed buffers (not pointers) so payload lifetime is self-contained.
 *
 * Events with no payload (LL_EV_PROVISION_START, LL_EV_FACTORY_RESET) take
 * NULL at the post site.
 */

typedef struct {
    bool on;
} ll_ev_power_toggle_payload_t;

typedef struct {
    uint32_t rgb;  /* 0xRRGGBB; upper byte ignored */
} ll_ev_base_color_payload_t;

typedef struct {
    char id[32];   /* null-terminated; truncated to fit ll_state_t.pattern_id */
} ll_ev_pattern_change_payload_t;

typedef struct {
    uint8_t value; /* 0-100; validation/snapping happens in handler layer, not here */
} ll_ev_brightness_payload_t;

typedef struct {
    ll_auth_mode_t mode;
    char secret[64];  /* empty string if not rotating; owned by auth/nvs downstream */
} ll_ev_auth_mode_payload_t;

typedef struct {
    bool enabled;
} ll_ev_telemetry_payload_t;

typedef struct {
    ll_event_t which;  /* which field just changed */
} ll_state_changed_payload_t;

typedef struct {
    char ssid[33];     /* 32-byte SSID + null terminator */
    uint8_t ip[4];
    int8_t rssi;       /* dBm; negative */
} ll_ev_wifi_connected_payload_t;

typedef struct {
    uint8_t reason;    /* wifi_err_reason_t from esp_wifi; forwarded as-is */
} ll_ev_wifi_disconnected_payload_t;

typedef struct {
    char ssid[33];     /* 32-byte SSID + null terminator */
    char password[65]; /* 64-byte WPA2 max + null; empty string = open net */
} ll_ev_wifi_apply_creds_payload_t;

/*
 * Handle to the dedicated state-bus event loop. Valid after
 * ll_state_bus_init(). Use with esp_event_handler_instance_register_with()
 * to subscribe to LL_EV_STATE_CHANGED or any specific LL_EV_* id.
 */
esp_event_loop_handle_t ll_state_bus_get_loop(void);
