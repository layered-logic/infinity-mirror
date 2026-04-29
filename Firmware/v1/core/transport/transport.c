/*
 * HTTP + WebSocket transport — ESP-IDF side.
 *
 * Server lifecycle:
 *   wifi up   → httpd_start on port 80, register /ws URI handler
 *   wifi down → httpd_stop (active WS connections get closed)
 *
 * WS handler dispatches on the envelope's `op` field. Session 1
 * implements only `ping`; everything else returns
 *   { ok: false, error: { code: "unknown_op", ... } }
 * to keep clients honest about which version of the protocol they're
 * talking to.
 *
 * Threading: start_server / stop_server run on the state-bus event
 * loop (single task → no races). The WS handler runs on the httpd
 * task; it only reads g_server transitively via the framework's
 * sockfd table, never touches it directly. Open auth mode means no
 * shared mutable state to guard yet.
 */

#include "transport.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"
#include "esp_app_desc.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "state_bus.h"
#include "state_bus_events.h"

static const char *TAG = "transport";

#define LL_TRANSPORT_PORT  80
#define LL_WS_RX_MAX       1024   /* envelope+payload — generous for ping/state */

static httpd_handle_t g_server;

/* ---- op handlers ------------------------------------------------- */

static cJSON *result_for_ping(void)
{
    cJSON *result = cJSON_CreateObject();
    if (!result) return NULL;

    const esp_app_desc_t *desc = esp_app_get_description();
    cJSON_AddStringToObject(result, "fw_version", desc->version);
    /* esp_timer_get_time returns microseconds since boot. */
    cJSON_AddNumberToObject(result, "uptime_s",
                            (double)(esp_timer_get_time() / 1000000));
    return result;
}

/* ---- envelope dispatch ------------------------------------------- */

static esp_err_t send_text_frame(httpd_req_t *req, const char *json, size_t len)
{
    httpd_ws_frame_t frame = {
        .final = true,
        .fragmented = false,
        .type = HTTPD_WS_TYPE_TEXT,
        .payload = (uint8_t *)json,
        .len = len,
    };
    return httpd_ws_send_frame(req, &frame);
}

static esp_err_t handle_envelope(httpd_req_t *req, const char *json)
{
    cJSON *envelope = cJSON_Parse(json);
    if (!envelope) {
        ESP_LOGW(TAG, "bad JSON envelope — dropping frame");
        return ESP_OK;  /* keep connection alive; client gets nothing */
    }

    const char *op     = cJSON_GetStringValue(cJSON_GetObjectItem(envelope, "op"));
    const char *req_id = cJSON_GetStringValue(cJSON_GetObjectItem(envelope, "req_id"));

    cJSON *resp = cJSON_CreateObject();
    if (!resp) {
        cJSON_Delete(envelope);
        return ESP_ERR_NO_MEM;
    }

    cJSON_AddStringToObject(resp, "op",     op     ? op     : "");
    cJSON_AddStringToObject(resp, "req_id", req_id ? req_id : "");

    if (op && strcmp(op, "ping") == 0) {
        cJSON_AddBoolToObject(resp, "ok", true);
        cJSON_AddItemToObject(resp, "result", result_for_ping());
        cJSON_AddNullToObject(resp, "error");
    } else {
        cJSON_AddBoolToObject(resp, "ok", false);
        cJSON_AddNullToObject(resp, "result");
        cJSON *err = cJSON_AddObjectToObject(resp, "error");
        cJSON_AddStringToObject(err, "code", "unknown_op");
        cJSON_AddStringToObject(err, "message",
            op ? "Op not implemented in this firmware version"
               : "Missing op field in envelope");
    }

    char *out = cJSON_PrintUnformatted(resp);
    cJSON_Delete(resp);
    cJSON_Delete(envelope);

    if (!out) return ESP_ERR_NO_MEM;

    esp_err_t err = send_text_frame(req, out, strlen(out));
    free(out);

    if (err != ESP_OK) {
        ESP_LOGW(TAG, "ws send: %s", esp_err_to_name(err));
    }
    return err;
}

/* ---- WS URI handler ---------------------------------------------- */

static esp_err_t ws_handler(httpd_req_t *req)
{
    /* esp_http_server invokes the registered URI handler twice per
     * connection: once for the upgrade handshake (method=HTTP_GET)
     * and again for each subsequent frame (other methods). */
    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "ws handshake from sock=%d", httpd_req_to_sockfd(req));
        return ESP_OK;
    }

    httpd_ws_frame_t frame = {0};
    frame.type = HTTPD_WS_TYPE_TEXT;

    /* Two-phase recv: max_len=0 fills frame.len without copying. */
    esp_err_t err = httpd_ws_recv_frame(req, &frame, 0);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "ws recv (size): %s", esp_err_to_name(err));
        return err;
    }

    if (frame.len == 0) {
        return ESP_OK;
    }
    if (frame.len > LL_WS_RX_MAX) {
        ESP_LOGW(TAG, "ws frame too large: %u bytes (max %d)",
                 (unsigned)frame.len, LL_WS_RX_MAX);
        return ESP_FAIL;
    }

    uint8_t *buf = calloc(1, frame.len + 1);  /* +1 for cJSON null term */
    if (!buf) return ESP_ERR_NO_MEM;
    frame.payload = buf;

    err = httpd_ws_recv_frame(req, &frame, frame.len);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "ws recv (payload): %s", esp_err_to_name(err));
        free(buf);
        return err;
    }

    if (frame.type == HTTPD_WS_TYPE_TEXT) {
        err = handle_envelope(req, (const char *)buf);
    }
    /* Binary / ping / pong frames: ignored at the app layer for
     * Session 1. The httpd layer handles WS protocol-level ping/pong
     * automatically when CONFIG_HTTPD_WS_SUPPORT=y. */

    free(buf);
    return err;
}

/* ---- lifecycle --------------------------------------------------- */

static const httpd_uri_t URI_WS = {
    .uri          = "/ws",
    .method       = HTTP_GET,
    .handler      = ws_handler,
    .user_ctx     = NULL,
    .is_websocket = true,
};

static esp_err_t start_server(void)
{
    if (g_server) return ESP_OK;

    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    cfg.server_port      = LL_TRANSPORT_PORT;
    cfg.max_uri_handlers = 4;     /* /ws today; HTTP REST adds more in S2 */
    cfg.lru_purge_enable = true;  /* close oldest socket on overflow */

    esp_err_t err = httpd_start(&g_server, &cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "httpd_start: %s", esp_err_to_name(err));
        g_server = NULL;
        return err;
    }

    err = httpd_register_uri_handler(g_server, &URI_WS);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "register %s: %s", URI_WS.uri, esp_err_to_name(err));
        httpd_stop(g_server);
        g_server = NULL;
        return err;
    }

    ESP_LOGI(TAG, "listening on :%d (ws %s)", LL_TRANSPORT_PORT, URI_WS.uri);
    return ESP_OK;
}

static void stop_server(void)
{
    if (!g_server) return;
    httpd_stop(g_server);
    g_server = NULL;
    ESP_LOGI(TAG, "stopped");
}

static void on_wifi_connected(void *arg, esp_event_base_t base,
                              int32_t id, void *data)
{
    (void)arg; (void)base; (void)id; (void)data;
    start_server();
}

static void on_wifi_disconnected(void *arg, esp_event_base_t base,
                                 int32_t id, void *data)
{
    (void)arg; (void)base; (void)id; (void)data;
    stop_server();
}

/* ---- public entry points ----------------------------------------- */

esp_err_t ll_transport_init(void)
{
    /* esp_http_server is started lazily on WIFI_CONNECTED. The init
     * call exists for symmetry with other modules and as a hook for
     * future per-boot setup (e.g. allocating the broadcast client
     * list once Session 2 lands). */
    ESP_LOGI(TAG, "init: server deferred until wifi up");
    return ESP_OK;
}

esp_err_t ll_transport_subscribe(void)
{
    esp_err_t err = esp_event_handler_instance_register_with(
        ll_state_bus_get_loop(),
        LL_STATE_EVENT_BASE, LL_EV_WIFI_CONNECTED,
        on_wifi_connected, NULL, NULL);
    if (err != ESP_OK) return err;

    return esp_event_handler_instance_register_with(
        ll_state_bus_get_loop(),
        LL_STATE_EVENT_BASE, LL_EV_WIFI_DISCONNECTED,
        on_wifi_disconnected, NULL, NULL);
}
