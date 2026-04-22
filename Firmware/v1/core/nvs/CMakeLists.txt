# core/nvs — settings persistence (ll_settings namespace).

idf_component_register(
    SRCS
        "nvs.c"
        "nvs_logic.c"
    INCLUDE_DIRS "."
    REQUIRES
        esp_event
        esp_timer
        nvs_flash
        state_bus
)
