#include "telemetry.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"
#include "esp_app_desc.h"
#include "esp_crt_bundle.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"

#include "state_bus.h"
#include "state_bus_events.h"

static const char *TAG = "telemetry";

#define LL_TELEMETRY_URL          "https://telemetry.layeredlogic.cc/v1/beacon"
#define LL_TELEMETRY_HTTP_TIMEOUT 8000   /* ms — short; mirror is mains-powered, not battery-bound */

/* 24h cadence with ±2h jitter so a million devices don't all phone home
 * on the same second. Range: [22h, 26h]. */
#define LL_TELEMETRY_PERIOD_BASE_US   ((int64_t)24 * 60 * 60 * 1000 * 1000)
#define LL_TELEMETRY_PERIOD_JITTER_US ((int64_t) 2 * 60 * 60 * 1000 * 1000)

/* First beacon fires 1-5 min after the first WIFI_CONNECTED so we have
 * time to settle (NTP sync, mdns publish, etc.) before the first POST. */
#define LL_TELEMETRY_FIRST_MIN_US ((int64_t) 60 * 1000 * 1000)
#define LL_TELEMETRY_FIRST_MAX_US ((int64_t)300 * 1000 * 1000)

static esp_timer_handle_t g_timer;
static bool               g_scheduled;       /* first WIFI_CONNECTED has fired */
static bool               g_panic_on_boot;   /* set in init if esp_reset_reason() == ESP_RST_PANIC */
static bool               g_panic_pending;   /* mirror of g_panic_on_boot; cleared after first successful beacon */

static const char *reset_reason_str(esp_reset_reason_t r)
{
    switch (r) {
    case ESP_RST_UNKNOWN:    return "UNKNOWN";
    case ESP_RST_POWERON:    return "POWERON";
    case ESP_RST_EXT:        return "EXT";
    case ESP_RST_SW:         return "SW";
    case ESP_RST_PANIC:      return "PANIC";
    case ESP_RST_INT_WDT:    return "INT_WDT";
    case ESP_RST_TASK_WDT:   return "TASK_WDT";
    case ESP_RST_WDT:        return "WDT";
    case ESP_RST_DEEPSLEEP:  return "DEEPSLEEP";
    case ESP_RST_BROWNOUT:   return "BROWNOUT";
    case ESP_RST_SDIO:       return "SDIO";
    case ESP_RST_USB:        return "USB";        /* USB peripheral reset (e.g. host-side toggle) */
    case ESP_RST_JTAG:       return "JTAG";       /* debug-probe reset */
    case ESP_RST_EFUSE:      return "EFUSE";      /* eFuse-driven reset (rare) */
    case ESP_RST_PWR_GLITCH: return "PWR_GLITCH"; /* C3-specific brownout-adjacent path */
    case ESP_RST_CPU_LOCKUP: return "CPU_LOCKUP"; /* lockup detector tripped */
    default:                 return "OTHER";
    }
}

/* Build the beacon JSON envelope. Returns a heap-allocated string the
 * caller must free, or NULL on alloc failure. */
static char *build_beacon_json(void)
{
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;

    const esp_app_desc_t *desc = esp_app_get_description();
    const ll_state_t *s = ll_state_bus_get();

    cJSON_AddNumberToObject(obj, "v",            1);
    cJSON_AddStringToObject(obj, "device_id",    ll_device_id_get());
    cJSON_AddStringToObject(obj, "fw_version",   desc ? desc->version : "?");
    cJSON_AddNumberToObject(obj, "uptime_s",     (double)(esp_timer_get_time() / 1000000));
    cJSON_AddNumberToObject(obj, "heap_free_min",
                            (double)esp_get_minimum_free_heap_size());
    cJSON_AddStringToObject(obj, "boot_reason",  reset_reason_str(esp_reset_reason()));

    /* RSSI: only meaningful in STA mode. esp_wifi_sta_get_ap_info fails
     * harmlessly if we're in SoftAP or disconnected; serialize 0 as a
     * sentinel for "unknown". The Worker validates rssi ∈ [-120, 0]
     * so 0 will be accepted; clients reading the data can treat 0 as
     * "no measurement available." */
    wifi_ap_record_t ap = {0};
    int rssi = (esp_wifi_sta_get_ap_info(&ap) == ESP_OK) ? ap.rssi : 0;
    cJSON_AddNumberToObject(obj, "rssi",         (double)rssi);

    cJSON_AddNumberToObject(obj, "led_count",    (double)s->led_count);

    if (g_panic_pending) {
        cJSON *p = cJSON_AddObjectToObject(obj, "panic");
        /* V1: only reset reason is captured. filename/line/task_name
         * needs a custom panic handler hook (esp_set_panic_handler) —
         * deferred. The Worker tolerates the partial payload because
         * panic is optional; if all three fields aren't present the
         * Worker will 400 the body, so we ship safe defaults. */
        cJSON_AddStringToObject(p, "filename",   "?");
        cJSON_AddNumberToObject(p, "line",       0);
        cJSON_AddStringToObject(p, "task_name",  "?");
    }

    char *json = cJSON_PrintUnformatted(obj);
    cJSON_Delete(obj);
    return json;
}

static void post_beacon(void)
{
    char *json = build_beacon_json();
    if (!json) {
        ESP_LOGW(TAG, "beacon: alloc failed");
        return;
    }

    esp_http_client_config_t cfg = {
        .url           = LL_TELEMETRY_URL,
        .method        = HTTP_METHOD_POST,
        .timeout_ms    = LL_TELEMETRY_HTTP_TIMEOUT,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    if (!client) {
        ESP_LOGW(TAG, "beacon: http client init failed");
        free(json);
        return;
    }

    esp_http_client_set_header(client, "Content-Type", "application/json");
    /* Cloudflare's default Bot Fight Mode rejects requests with bare /
     * empty / library-default User-Agent strings (returns 403, CF error
     * code 1010). Identify ourselves explicitly so the beacon path
     * isn't filtered. The string also doubles as a discoverable
     * version stamp in CF analytics. */
    esp_http_client_set_header(client, "User-Agent", "ll-mirror/1 esp32c3");
    esp_http_client_set_post_field(client, json, (int)strlen(json));

    esp_err_t err = esp_http_client_perform(client);
    int status = esp_http_client_get_status_code(client);
    if (err == ESP_OK && status >= 200 && status < 300) {
        ESP_LOGI(TAG, "beacon sent: status=%d (%u bytes)", status, (unsigned)strlen(json));
        g_panic_pending = false;
    } else {
        ESP_LOGW(TAG, "beacon failed: err=%s status=%d",
                 esp_err_to_name(err), status);
    }

    esp_http_client_cleanup(client);
    free(json);
}

static int64_t next_period_us(void)
{
    /* Uniform jitter in [-LL_TELEMETRY_PERIOD_JITTER_US, +LL_TELEMETRY_PERIOD_JITTER_US]. */
    uint32_t r = esp_random();
    int64_t jitter = (int64_t)(r % (uint32_t)(2 * LL_TELEMETRY_PERIOD_JITTER_US))
                    - LL_TELEMETRY_PERIOD_JITTER_US;
    return LL_TELEMETRY_PERIOD_BASE_US + jitter;
}

static void schedule_next(int64_t period_us)
{
    esp_err_t err = esp_timer_start_once(g_timer, (uint64_t)period_us);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "timer start: %s", esp_err_to_name(err));
    }
}

static void beacon_timer_cb(void *arg)
{
    (void)arg;
    const ll_state_t *s = ll_state_bus_get();
    if (s->telemetry_enabled) {
        post_beacon();
    } else {
        ESP_LOGD(TAG, "telemetry disabled — skipping beacon, will check again next cycle");
    }
    schedule_next(next_period_us());
}

static void on_wifi_connected(void *arg, esp_event_base_t base,
                               int32_t id, void *data)
{
    (void)arg; (void)base; (void)id; (void)data;
    if (g_scheduled) return;  /* idempotent across reconnects */
    g_scheduled = true;

    /* First fire is 1-5 min after wifi-up. */
    uint32_t r = esp_random();
    int64_t span_us = LL_TELEMETRY_FIRST_MAX_US - LL_TELEMETRY_FIRST_MIN_US;
    int64_t first_us = LL_TELEMETRY_FIRST_MIN_US + (int64_t)(r % (uint32_t)span_us);
    ESP_LOGI(TAG, "first beacon scheduled in %" PRId64 " s", first_us / 1000000);
    schedule_next(first_us);
}

esp_err_t ll_telemetry_init(void)
{
    g_panic_on_boot = (esp_reset_reason() == ESP_RST_PANIC);
    g_panic_pending = g_panic_on_boot;
    if (g_panic_on_boot) {
        ESP_LOGW(TAG, "boot reason = PANIC; will report in next beacon");
    }

    const esp_timer_create_args_t args = {
        .callback = beacon_timer_cb,
        .name     = "ll_telemetry",
    };
    return esp_timer_create(&args, &g_timer);
}

esp_err_t ll_telemetry_subscribe(void)
{
    return esp_event_handler_instance_register_with(
        ll_state_bus_get_loop(),
        LL_STATE_EVENT_BASE, LL_EV_WIFI_CONNECTED,
        on_wifi_connected, NULL, NULL);
}
