/*
 * Unified state bus — single source of truth for device state.
 *
 * Concurrency model (firmware-spec.md §3):
 *   - One dedicated esp_event loop, task pinned to core 0.
 *   - Single writer: ll_state_t is only mutated by on_state_event(), which
 *     runs on that loop's task. Publishers never touch ll_state_t directly.
 *   - Readers (ll_state_bus_get) return a const pointer and race against
 *     the writer by design — eventual consistency. Fields are small enough
 *     that word-size reads are atomic on Xtensa/RISC-V; no locks.
 *   - Publishers may call ll_state_bus_post() from any task or from an
 *     ISR; ISR context is detected and routed through esp_event_isr_post_to.
 *
 * After an incoming LL_EV_* is applied, the handler re-dispatches
 * LL_EV_STATE_CHANGED on the same loop so subscribers (pattern_interp,
 * transport, nvs, matter_bridge) react without having to duplicate the
 * apply logic.
 */

#include "state_bus.h"
#include "state_bus_events.h"

#include <stdio.h>
#include <string.h>

#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

static const char *TAG = "state_bus";

ESP_EVENT_DEFINE_BASE(LL_STATE_EVENT_BASE);

static ll_state_t g_state;
static esp_event_loop_handle_t g_loop;
static bool g_initialized;
static char g_id_suffix[7];  /* lower 3 bytes of WiFi STA MAC, lowercase hex */

static size_t payload_size_for(ll_event_t ev)
{
    switch (ev) {
    case LL_EV_POWER_TOGGLE:    return sizeof(ll_ev_power_toggle_payload_t);
    case LL_EV_BASE_COLOR:      return sizeof(ll_ev_base_color_payload_t);
    case LL_EV_PATTERN_CHANGE:  return sizeof(ll_ev_pattern_change_payload_t);
    case LL_EV_BRIGHTNESS:      return sizeof(ll_ev_brightness_payload_t);
    case LL_EV_AUTH_MODE:       return sizeof(ll_ev_auth_mode_payload_t);
    case LL_EV_TELEMETRY:       return sizeof(ll_ev_telemetry_payload_t);
    case LL_EV_NAME_CHANGE:     return sizeof(ll_ev_name_change_payload_t);
    case LL_EV_PROVISION_START:
    case LL_EV_FACTORY_RESET:   return 0;
    }
    return 0;
}

/* Brightness restored on power-on when current brightness is 0 (e.g.
 * after a webapp `{brightness: 0}` set, then a button single-press to
 * power back on). Matches the boot default so first-power-on after a
 * factory reset behaves consistently. Closes the on/brightness wire
 * contradiction from post-mini-sprint-bugs.md #2. */
#define LL_BRIGHTNESS_RESUME_DEFAULT 75

static void apply_event(ll_event_t ev, const void *payload)
{
    switch (ev) {
    case LL_EV_POWER_TOGGLE: {
        const ll_ev_power_toggle_payload_t *p = payload;
        g_state.on = p->on;
        /* Invariant: on == true ⟹ brightness > 0. If we're powering on
         * but brightness is currently 0 (e.g. last action was webapp
         * `{brightness: 0}` which couples to !on), restore to the
         * resume default rather than render a dark "on" state.
         * Power-off does NOT zero brightness — UX preserves the user's
         * setting across power-cycle; the renderer gates on `on` so a
         * preserved brightness with on=false still renders dark. */
        if (p->on && g_state.brightness == 0) {
            g_state.brightness = LL_BRIGHTNESS_RESUME_DEFAULT;
        }
        break;
    }
    case LL_EV_BASE_COLOR: {
        const ll_ev_base_color_payload_t *p = payload;
        g_state.base_color_rgb = p->rgb & 0x00FFFFFFu;
        break;
    }
    case LL_EV_PATTERN_CHANGE: {
        const ll_ev_pattern_change_payload_t *p = payload;
        strncpy(g_state.pattern_id, p->id, sizeof(g_state.pattern_id) - 1);
        g_state.pattern_id[sizeof(g_state.pattern_id) - 1] = '\0';
        break;
    }
    case LL_EV_BRIGHTNESS: {
        const ll_ev_brightness_payload_t *p = payload;
        g_state.brightness = p->value;
        /* Invariant: brightness == 0 ⟺ !on. Treat brightness=0 as an
         * implicit power-off; treat brightness>0 as an implicit
         * power-on. Lets clients send a single-field `{brightness: 0}`
         * without having to also send `{on: false}` and vice versa. */
        g_state.on = (p->value > 0);
        break;
    }
    case LL_EV_AUTH_MODE: {
        const ll_ev_auth_mode_payload_t *p = payload;
        g_state.auth_mode = p->mode;
        /* secret rotation is the auth/nvs module's job, not state_bus's. */
        break;
    }
    case LL_EV_TELEMETRY: {
        const ll_ev_telemetry_payload_t *p = payload;
        g_state.telemetry_enabled = p->enabled;
        break;
    }
    case LL_EV_NAME_CHANGE: {
        const ll_ev_name_change_payload_t *p = payload;
        strncpy(g_state.name, p->name, sizeof(g_state.name) - 1);
        g_state.name[sizeof(g_state.name) - 1] = '\0';
        break;
    }
    case LL_EV_PROVISION_START:
        /* No ll_state_t mutation: drives side effects in provisioning/.
         * Subscribers still get the notify. */
        break;
    case LL_EV_FACTORY_RESET:
        /* Reset settings in-memory to compiled defaults. The nvs subscriber
         * will see LL_EV_STATE_CHANGED with which=LL_EV_FACTORY_RESET and
         * erase the flash namespace instead of saving defaults back — so
         * the absent blob survives reboot, not a written "defaults" blob. */
        ll_state_defaults(&g_state);
        break;
    }
}

static void on_state_event(void *arg, esp_event_base_t base,
                           int32_t id, void *data)
{
    (void)arg;
    (void)base;

    if (id >= LL_EV_STATE_CHANGED) {
        /* Our own re-dispatch (LL_EV_STATE_CHANGED), or a transient
         * event (LL_EV_RECESSED_HOLD_BEGIN/END) that rides the state-bus
         * loop as a convenient fan-out channel but isn't a state mutation.
         * Subscribers handle these; we don't. */
        return;
    }

    ll_event_t ev = (ll_event_t)id;
    apply_event(ev, data);

    ll_state_changed_payload_t out = { .which = ev };
    esp_err_t err = esp_event_post_to(g_loop, LL_STATE_EVENT_BASE,
                                      LL_EV_STATE_CHANGED,
                                      &out, sizeof(out), 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "re-dispatch of LL_EV_STATE_CHANGED failed: %s",
                 esp_err_to_name(err));
    }
}

void ll_state_bus_init(const ll_state_t *initial)
{
    if (g_initialized) {
        return;
    }

    if (initial) {
        g_state = *initial;
    } else {
        ll_state_defaults(&g_state);
    }

    /* Self-heal the on/brightness invariant on boot. A legacy NVS blob
     * from a pre-fix era (post-mini-sprint-bugs.md #2, fixed via
     * apply_event coupling) could carry the contradictory state where
     * on==true with brightness==0. apply_event prevents any new mutation
     * from producing that state, but the boot path skips apply_event —
     * so we correct it here once. Mirrors the LL_EV_POWER_TOGGLE branch
     * of apply_event: if on but brightness=0, restore to resume default. */
    if (g_state.on && g_state.brightness == 0) {
        g_state.brightness = LL_BRIGHTNESS_RESUME_DEFAULT;
    }
    /* Symmetric case (on=false, brightness>0) is fine — it represents
     * "powered off but the user's brightness setting is preserved",
     * which is a valid intentional state. The renderer gates on `on`
     * so this still renders dark. */

    /* Cache the MAC-derived id suffix for lookups via ll_device_id_get().
     * Same form as ll_mdns publishes: lower 3 bytes, lowercase hex. */
    uint8_t mac[6];
    if (esp_read_mac(mac, ESP_MAC_WIFI_STA) == ESP_OK) {
        snprintf(g_id_suffix, sizeof(g_id_suffix), "%02x%02x%02x",
                 mac[3], mac[4], mac[5]);
    } else {
        g_id_suffix[0] = '\0';
    }

    const esp_event_loop_args_t args = {
        .queue_size = 16,
        .task_name = "state_bus",
        .task_priority = tskIDLE_PRIORITY + 5,
        .task_stack_size = 4096,
        .task_core_id = 0,
    };
    ESP_ERROR_CHECK(esp_event_loop_create(&args, &g_loop));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
        g_loop, LL_STATE_EVENT_BASE, ESP_EVENT_ANY_ID,
        on_state_event, NULL, NULL));

    g_initialized = true;
    ESP_LOGI(TAG,
             "init: on=%d pattern=%s color=#%06lX brightness=%u leds=%u",
             (int)g_state.on, g_state.pattern_id,
             (unsigned long)g_state.base_color_rgb,
             (unsigned)g_state.brightness, (unsigned)g_state.led_count);
}

const ll_state_t *ll_state_bus_get(void)
{
    return &g_state;
}

void ll_state_bus_post(ll_event_t ev, const void *payload)
{
    const size_t sz = payload_size_for(ev);
    esp_err_t err;

    if (xPortInIsrContext()) {
        BaseType_t yield = pdFALSE;
        err = esp_event_isr_post_to(g_loop, LL_STATE_EVENT_BASE,
                                    (int32_t)ev, payload, sz, &yield);
        (void)err;  /* ISR path: no blocking log; drop silently on failure. */
        if (yield == pdTRUE) {
            portYIELD_FROM_ISR();
        }
    } else {
        err = esp_event_post_to(g_loop, LL_STATE_EVENT_BASE,
                                (int32_t)ev, payload, sz, 0);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "post ev=%d failed: %s",
                     (int)ev, esp_err_to_name(err));
        }
    }
}

esp_event_loop_handle_t ll_state_bus_get_loop(void)
{
    return g_loop;
}

const char *ll_device_id_get(void)
{
    return g_id_suffix;
}
