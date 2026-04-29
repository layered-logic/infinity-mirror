# core/transport — HTTP + WebSocket transport.
#
# Component name `transport` is unique in the IDF graph (the built-in
# tcp_transport is registered as `tcp_transport`, not `transport`),
# so no folder rename is needed unlike core/ll_mdns/.
#
# WS handling depends on CONFIG_HTTPD_WS_SUPPORT=y, which the project
# sdkconfig.defaults sets. Without it, httpd_register_uri_handler will
# accept the registration but the upgrade handshake won't work.

idf_component_register(
    SRCS "transport.c"
    INCLUDE_DIRS "."
    REQUIRES
        esp_app_format
        esp_event
        esp_http_server
        esp_timer
        json
        state_bus
)
