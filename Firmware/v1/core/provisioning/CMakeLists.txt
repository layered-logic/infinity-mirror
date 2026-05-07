# core/provisioning — Wi-Fi station + wifi_prov_mgr wrapper.

idf_component_register(
    SRCS "provisioning.c"
    INCLUDE_DIRS "."
    REQUIRES
        esp_event
        esp_netif
        esp_timer
        esp_wifi
        ll_wifi
        nvs_flash
        state_bus
        wifi_provisioning
)
