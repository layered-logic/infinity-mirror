#pragma once

/*
 * Captive-portal DNS hijack.
 *
 * When the device is in SoftAP mode, runs a tiny UDP responder on
 * port 53 that answers every A-record query with 192.168.4.1 (the
 * SoftAP gateway). This makes the host OS's captive-portal probe URLs
 * (Android connectivitycheck.gstatic.com, iOS captive.apple.com,
 * Windows msftconnecttest.com, etc.) all resolve to the device, so
 * the OS surfaces its "sign in to network" sheet and auto-opens the
 * webapp.
 *
 * Lifecycle:
 *   - LL_EV_WIFI_CONNECTED with WIFI_MODE_AP active → start the
 *     responder task (binds INADDR_ANY:53, but only the AP netif
 *     receives queries since STA isn't up).
 *   - LL_EV_WIFI_DISCONNECTED, or any transition out of AP mode →
 *     stop the task, close the socket. We don't want to intercept
 *     real DNS queries when the device is a station on a real
 *     network.
 *
 * Out of scope:
 *   - HTTPS captive-portal probes (newer iOS / ChromeOS) — can't be
 *     intercepted without per-device TLS certs. Those clients fall
 *     back to the older HTTP probe after ~5-10s, so the sheet still
 *     appears, just slower.
 *   - Recursive DNS resolution. Every query gets the same answer.
 *
 * Wiring order in main():
 *     ll_captive_dns_init()
 *     ll_captive_dns_subscribe()  (after state_bus is up)
 */

#include "esp_err.h"

esp_err_t ll_captive_dns_init(void);
esp_err_t ll_captive_dns_subscribe(void);
