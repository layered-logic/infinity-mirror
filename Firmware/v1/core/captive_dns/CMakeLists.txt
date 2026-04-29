# core/captive_dns — UDP DNS responder that hijacks every A query to
# point at the SoftAP gateway. Used to trigger OS captive-portal sheets
# when the device is in SoftAP mode for first-time pairing.
#
# Only active in AP / APSTA mode (gated on esp_wifi_get_mode at start).
# Uses lwip's BSD-socket API for portability — no esp_netif-specific
# coupling needed since we bind INADDR_ANY:53 and only the AP netif is
# up when this runs.

idf_component_register(
    SRCS "captive_dns.c"
    INCLUDE_DIRS "."
    REQUIRES
        esp_event
        esp_wifi
        lwip
        state_bus
)
