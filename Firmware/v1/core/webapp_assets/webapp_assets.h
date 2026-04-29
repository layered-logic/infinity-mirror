#pragma once

/*
 * Webapp asset server.
 *
 * Embeds the gzipped Vite production bundle (the .gz files under
 * Firmware/v1/webapp/dist) into the firmware image and registers static-
 * file URI handlers on the existing esp_http_server instance:
 *
 *   GET /         → index.html.gz   (text/html)
 *   GET /app.js   → app.js.gz       (application/javascript)
 *   GET /app.css  → app.css.gz      (text/css)
 *
 * All three are served with Content-Encoding: gzip so the browser
 * decompresses on receipt. Cache-Control: no-cache for now — webapp ships
 * embedded in firmware, so a fresh fetch on every load matches "OTA
 * pushes the new webapp" semantics. Tighter caching (max-age, ETag) is a
 * future optimization once we want to save battery on phones.
 *
 * Build prerequisite: `npm run build` in Firmware/v1/webapp/ must have
 * run before the firmware build. The npm build script gzips each dist
 * file into a .gz sibling; the IDF component's CMakeLists EMBED_FILES
 * references those .gz outputs and will fail at configure time if any
 * are missing.
 *
 * Wiring: transport.c calls ll_webapp_assets_register(g_server) inside
 * start_server(), after httpd_start() and after the /ws handler is
 * registered.
 */

#include "esp_err.h"
#include "esp_http_server.h"

esp_err_t ll_webapp_assets_register(httpd_handle_t server);
