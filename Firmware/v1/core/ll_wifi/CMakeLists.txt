# core/ll_wifi — multi-network Wi-Fi credential store (ll_wifi namespace).

idf_component_register(
    SRCS
        "ll_wifi.c"
        "ll_wifi_logic.c"
    INCLUDE_DIRS "."
    REQUIRES
        nvs_flash
)
