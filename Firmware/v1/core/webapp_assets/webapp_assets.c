/*
 * Webapp asset server — see webapp_assets.h for the contract.
 *
 * Each embedded asset gets a small handler that just emits the right
 * Content-Type + Content-Encoding: gzip header pair and ships the bytes
 * straight from flash. No copying, no decompression on-device.
 *
 * EMBED_FILES symbols (linker-generated): for `dist/index.html.gz` the
 * symbols are `_binary_index_html_gz_start` / `_binary_index_html_gz_end`.
 * The IDF docs say to declare them as `extern const uint8_t [] asm(...)`,
 * but the simpler `extern const uint8_t name[]` form works on
 * xtensa-elf and riscv32-esp-elf as long as the linker symbol name
 * matches verbatim — which it does for these three.
 */

#include "webapp_assets.h"

#include <string.h>

#include "esp_log.h"

static const char *TAG = "webapp_assets";

/* ---- embedded asset symbols -------------------------------------- */

extern const uint8_t index_html_gz_start[] asm("_binary_index_html_gz_start");
extern const uint8_t index_html_gz_end[]   asm("_binary_index_html_gz_end");
extern const uint8_t app_js_gz_start[]     asm("_binary_app_js_gz_start");
extern const uint8_t app_js_gz_end[]       asm("_binary_app_js_gz_end");
extern const uint8_t app_css_gz_start[]    asm("_binary_app_css_gz_start");
extern const uint8_t app_css_gz_end[]      asm("_binary_app_css_gz_end");

/* ---- handler ----------------------------------------------------- */

typedef struct {
    const char    *content_type;
    const uint8_t *body_start;
    const uint8_t *body_end;
} asset_t;

static esp_err_t serve_asset(httpd_req_t *req, const asset_t *a)
{
    httpd_resp_set_type(req, a->content_type);
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    /* Cache-Control deliberately defensive: webapp ships inside firmware,
     * so an OTA bumping fw_version is the only way the bundle changes.
     * Letting browsers cache aggressively is fine post-demo; for now keep
     * dev iteration trivial by forcing a refetch each load. */
    httpd_resp_set_hdr(req, "Cache-Control", "no-cache");

    const size_t len = (size_t)(a->body_end - a->body_start);
    return httpd_resp_send(req, (const char *)a->body_start, len);
}

/* Per-URI thunks. ctx-based dispatch would be tidier, but esp_http_server
 * stores user_ctx as a void* on the URI struct and we'd lose the literal
 * benefit anyway since the handler still has to switch on the asset. */
static esp_err_t h_index(httpd_req_t *req)
{
    static const asset_t a = {
        .content_type = "text/html; charset=utf-8",
        .body_start   = index_html_gz_start,
        .body_end     = index_html_gz_end,
    };
    return serve_asset(req, &a);
}

static esp_err_t h_app_js(httpd_req_t *req)
{
    static const asset_t a = {
        .content_type = "application/javascript; charset=utf-8",
        .body_start   = app_js_gz_start,
        .body_end     = app_js_gz_end,
    };
    return serve_asset(req, &a);
}

static esp_err_t h_app_css(httpd_req_t *req)
{
    static const asset_t a = {
        .content_type = "text/css; charset=utf-8",
        .body_start   = app_css_gz_start,
        .body_end     = app_css_gz_end,
    };
    return serve_asset(req, &a);
}

/* ---- captive-portal 404 handler ---------------------------------- */

/* Catch every URL we don't already serve and respond with a redirect to
 * the webapp root. This is what makes OS captive-portal probes pop the
 * sign-in sheet: Android's connectivitycheck.gstatic.com/generate_204,
 * iOS's captive.apple.com/hotspot-detect.html, Windows' connecttest.txt,
 * etc. all fail to match any URI in our table and fall through here.
 *
 * The 302 has Location set to a relative root path rather than a full
 * URL on purpose — the OS probe was made with the captive-portal
 * domain in the Host header, so the browser/OS auto-completes the URL
 * against that host (resolved by our DNS hijack to 192.168.4.1).
 * Returning a relative URL means the browser's address bar shows the
 * probe domain instead of an explicit IP, which keeps the captive
 * sheet UX consistent across phone OSes.
 *
 * STA-mode caveat: this handler is global (esp_http_server doesn't
 * support per-mode URI tables). If someone hits an unknown path on the
 * device when it's a real station on a real network, they'll be
 * redirected to /. That's a reasonable fallback for a misspelled URL,
 * not a bug. */
static esp_err_t cap_404_redirect(httpd_req_t *req, httpd_err_code_t err)
{
    (void)err;
    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    return httpd_resp_send(req, NULL, 0);
}

/* ---- registration ------------------------------------------------ */

esp_err_t ll_webapp_assets_register(httpd_handle_t server)
{
    if (!server) return ESP_ERR_INVALID_ARG;

    static const httpd_uri_t URIS[] = {
        { .uri = "/",        .method = HTTP_GET, .handler = h_index,   .user_ctx = NULL },
        { .uri = "/app.js",  .method = HTTP_GET, .handler = h_app_js,  .user_ctx = NULL },
        { .uri = "/app.css", .method = HTTP_GET, .handler = h_app_css, .user_ctx = NULL },
    };

    for (size_t i = 0; i < sizeof(URIS) / sizeof(URIS[0]); i++) {
        esp_err_t err = httpd_register_uri_handler(server, &URIS[i]);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "register %s: %s", URIS[i].uri, esp_err_to_name(err));
            return err;
        }
    }

    esp_err_t err = httpd_register_err_handler(server, HTTPD_404_NOT_FOUND,
                                               cap_404_redirect);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "register 404 redirect: %s", esp_err_to_name(err));
        return err;
    }

    const size_t total = (size_t)(index_html_gz_end - index_html_gz_start)
                       + (size_t)(app_js_gz_end     - app_js_gz_start)
                       + (size_t)(app_css_gz_end    - app_css_gz_start);
    ESP_LOGI(TAG, "registered /  /app.js  /app.css  + 404 captive redirect "
                  "(%u bytes gzipped total)",
             (unsigned)total);
    return ESP_OK;
}
