#pragma once

/*
 * HTTP + WebSocket transport.
 *
 * Hosts the wire protocol from docs/control-protocol-spec.md over
 * esp_http_server. Listens on port 80 with a WebSocket endpoint at
 * /ws and static handlers for the embedded webapp (registered by
 * core/webapp_assets/). Server starts lazily on LL_EV_WIFI_CONNECTED
 * and stops on LL_EV_WIFI_DISCONNECTED — no point listening on a
 * netif we don't have an IP on.
 *
 * V1 op surface (control-protocol-spec §4): `ping`, `get_state`,
 * `set_state`, `set_wifi_creds`, `factory_reset`. Other ops in the
 * spec (`get_info`, `set_auth_mode`, `list_patterns`, OTA, telemetry)
 * are V2. HTTP REST mirror of the WS surface and HMAC verification
 * (paired-mode auth) are deferred.
 *
 * Wiring order in main():
 *     ll_provisioning_init()    → brings up esp_netif + configures SoftAP
 *     ll_state_bus_init()
 *     ll_mdns_init() / ll_mdns_subscribe()
 *     ll_transport_init()       → no server yet (deferred)
 *     ll_transport_subscribe()  → wires WIFI_CONNECTED → start_server
 */

#include "esp_err.h"

esp_err_t ll_transport_init(void);
esp_err_t ll_transport_subscribe(void);
