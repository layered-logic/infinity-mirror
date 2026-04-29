#pragma once

/*
 * HTTP + WebSocket transport.
 *
 * Hosts the wire protocol from docs/control-protocol-spec.md over
 * esp_http_server. Listens on port 80 with a WebSocket endpoint at
 * /ws. Server starts lazily on LL_EV_WIFI_CONNECTED and stops on
 * LL_EV_WIFI_DISCONNECTED — no point listening on a netif we don't
 * have an IP on.
 *
 * Session 1 scope: WebSocket endpoint + `ping` op only. Session 2
 * picks up state.get / state.set / state.subscribe + state-change
 * broadcasts. HTTP REST surface and HMAC verification deferred.
 *
 * Wiring order in main():
 *     ll_provisioning_init()      → brings up esp_netif
 *     ll_state_bus_init()
 *     ll_mdns_init() / ll_mdns_subscribe()
 *     ll_transport_init()         → no server yet (deferred)
 *     ll_transport_subscribe()    → wires WIFI_CONNECTED → start_server
 */

#include "esp_err.h"

esp_err_t ll_transport_init(void);
esp_err_t ll_transport_subscribe(void);
