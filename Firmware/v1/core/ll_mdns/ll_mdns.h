#pragma once

/*
 * mDNS service publisher for the Layered Logic mirror.
 *
 * Responsibility (control-protocol-spec §2.2): advertise the device on
 * the local network so app and webapp clients can discover it without
 * manual IP entry. Published record:
 *
 *   Service:   _layeredlogic._tcp.local. on port 80
 *   Hostname:  layered-logic-mirror-XXXXXX.local
 *              (XXXXXX = lower 3 bytes of the WiFi STA MAC, hex)
 *   TXT:       variant=<standard|matter>
 *              version=<fw version, e.g. 73b1385>
 *              id=<XXXXXX, the same suffix as the hostname>
 *              auth=<open|paired>
 *
 * The service is only advertised while Wi-Fi is up. On disconnect the
 * record is removed so clients don't cache an entry pointing at a stale
 * IP. The auth TXT is the only record that mutates at runtime; the
 * module republishes it on LL_EV_AUTH_MODE.
 *
 * Header is named ll_mdns.h (rather than mdns.h) to avoid collision
 * with the espressif/mdns component's own mdns.h public header.
 *
 * Wiring order in main():
 *     ll_provisioning_init()    → brings up esp_netif (prereq for mdns_init)
 *     ll_state_bus_init()
 *     ll_mdns_init()            → mdns_init + hostname; service deferred
 *     ll_mdns_subscribe()       → wires WIFI_CONNECTED → publish, etc.
 */

#include "esp_err.h"

esp_err_t ll_mdns_init(void);
esp_err_t ll_mdns_subscribe(void);
