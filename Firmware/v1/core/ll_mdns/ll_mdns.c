/*
 * mDNS service publisher — ESP-IDF side.
 *
 * Wraps the espressif/mdns managed component. Lifecycle:
 *
 *   boot          → ll_mdns_init: mdns_init + hostname/instance fixed
 *                                  for the lifetime of the device.
 *   wifi up       → publish _layeredlogic._tcp record on port 80.
 *   wifi down     → remove the record. Don't leave stale advertisements
 *                   pointing at an IP that no longer routes.
 *   auth_mode set → republish only the auth TXT (single key update,
 *                   no full service re-add).
 *
 * Three of the four TXT records are fixed at boot (variant from the
 * compile-time LL_VARIANT_NAME, version from esp_app_get_description,
 * id from MAC) so they're cached at init.
 */

#include "ll_mdns.h"

#include <stdio.h>
#include <string.h>

#include "esp_app_desc.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_wifi.h"

#include "mdns.h"

#include "state_bus.h"
#include "state_bus_events.h"

static const char *TAG = "mdns";

#define LL_MDNS_SERVICE_TYPE   "_layeredlogic"
#define LL_MDNS_PROTO          "_tcp"
#define LL_MDNS_PORT           80
#define LL_MDNS_HOSTNAME_PFX   "layered-logic-mirror-"
#define LL_MDNS_INSTANCE       "Layered Logic Mirror"

#ifndef LL_VARIANT_NAME
/* Top-level CMakeLists.txt injects this. The fallback only fires if the
 * macro definition is dropped on the floor — flag it loudly rather than
 * silently advertising an empty string. */
#define LL_VARIANT_NAME "unknown"
#endif

/* g_id_suffix: 6 hex chars + null. g_hostname: prefix(21) + suffix(6) + null.
 * g_fw_version: esp_app_desc_t::version is char[32]; +1 for null. */
static char g_id_suffix[7];
static char g_hostname[40];
static char g_fw_version[33];
static bool g_service_published;

static const char *auth_string(ll_auth_mode_t mode)
{
    return mode == LL_AUTH_PAIRED ? "paired" : "open";
}

static void cache_identity(void)
{
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    snprintf(g_id_suffix, sizeof(g_id_suffix), "%02x%02x%02x",
             mac[3], mac[4], mac[5]);
    snprintf(g_hostname, sizeof(g_hostname), LL_MDNS_HOSTNAME_PFX "%s",
             g_id_suffix);

    const esp_app_desc_t *desc = esp_app_get_description();
    strncpy(g_fw_version, desc->version, sizeof(g_fw_version) - 1);
    g_fw_version[sizeof(g_fw_version) - 1] = '\0';
}

static esp_err_t publish_service(void)
{
    if (g_service_published) {
        return ESP_OK;
    }

    const ll_state_t *s = ll_state_bus_get();
    mdns_txt_item_t txt[] = {
        { "variant", LL_VARIANT_NAME       },
        { "version", g_fw_version          },
        { "id",      g_id_suffix           },
        { "auth",    auth_string(s->auth_mode) },
    };

    esp_err_t err = mdns_service_add(LL_MDNS_INSTANCE,
                                     LL_MDNS_SERVICE_TYPE, LL_MDNS_PROTO,
                                     LL_MDNS_PORT,
                                     txt, sizeof(txt) / sizeof(txt[0]));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mdns_service_add: %s", esp_err_to_name(err));
        return err;
    }

    g_service_published = true;
    ESP_LOGI(TAG, "published %s.%s.local:%d (variant=%s version=%s id=%s auth=%s)",
             LL_MDNS_SERVICE_TYPE, LL_MDNS_PROTO, LL_MDNS_PORT,
             LL_VARIANT_NAME, g_fw_version, g_id_suffix,
             auth_string(s->auth_mode));
    return ESP_OK;
}

static void remove_service(void)
{
    if (!g_service_published) {
        return;
    }
    esp_err_t err = mdns_service_remove(LL_MDNS_SERVICE_TYPE, LL_MDNS_PROTO);
    if (err != ESP_OK && err != ESP_ERR_NOT_FOUND) {
        ESP_LOGW(TAG, "mdns_service_remove: %s", esp_err_to_name(err));
    }
    g_service_published = false;
    ESP_LOGI(TAG, "service removed");
}

static void republish_auth(void)
{
    if (!g_service_published) {
        return;
    }
    const ll_state_t *s = ll_state_bus_get();
    esp_err_t err = mdns_service_txt_item_set(
        LL_MDNS_SERVICE_TYPE, LL_MDNS_PROTO, "auth", auth_string(s->auth_mode));
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "auth TXT update: %s", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(TAG, "auth TXT -> %s", auth_string(s->auth_mode));
}

static void on_wifi_connected(void *arg, esp_event_base_t base,
                              int32_t id, void *data)
{
    (void)arg; (void)base; (void)id;
    const ll_ev_wifi_connected_payload_t *p = data;
    ESP_LOGI(TAG, "wifi up (ssid=%s ip=%u.%u.%u.%u rssi=%d) — publishing",
             p->ssid,
             (unsigned)p->ip[0], (unsigned)p->ip[1],
             (unsigned)p->ip[2], (unsigned)p->ip[3],
             (int)p->rssi);
    publish_service();
}

static void on_wifi_disconnected(void *arg, esp_event_base_t base,
                                 int32_t id, void *data)
{
    (void)arg; (void)base; (void)id; (void)data;
    remove_service();
}

static void on_state_changed(void *arg, esp_event_base_t base,
                             int32_t id, void *data)
{
    (void)arg; (void)base; (void)id;
    const ll_state_changed_payload_t *p = data;
    if (p->which == LL_EV_AUTH_MODE) {
        republish_auth();
    }
}

esp_err_t ll_mdns_init(void)
{
    cache_identity();

    esp_err_t err = mdns_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mdns_init: %s", esp_err_to_name(err));
        return err;
    }

    err = mdns_hostname_set(g_hostname);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mdns_hostname_set: %s", esp_err_to_name(err));
        return err;
    }

    err = mdns_instance_name_set(LL_MDNS_INSTANCE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mdns_instance_name_set: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "init: hostname=%s.local (service deferred until wifi up)",
             g_hostname);
    return ESP_OK;
}

esp_err_t ll_mdns_subscribe(void)
{
    esp_err_t err = esp_event_handler_instance_register_with(
        ll_state_bus_get_loop(),
        LL_STATE_EVENT_BASE, LL_EV_WIFI_CONNECTED,
        on_wifi_connected, NULL, NULL);
    if (err != ESP_OK) return err;

    err = esp_event_handler_instance_register_with(
        ll_state_bus_get_loop(),
        LL_STATE_EVENT_BASE, LL_EV_WIFI_DISCONNECTED,
        on_wifi_disconnected, NULL, NULL);
    if (err != ESP_OK) return err;

    err = esp_event_handler_instance_register_with(
        ll_state_bus_get_loop(),
        LL_STATE_EVENT_BASE, LL_EV_STATE_CHANGED,
        on_state_changed, NULL, NULL);
    return err;
}
