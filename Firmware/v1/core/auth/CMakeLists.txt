# core/auth — paired-mode authentication (ll_auth namespace + HMAC verify).

idf_component_register(
    SRCS
        "auth.c"
        "auth_logic.c"
    INCLUDE_DIRS "."
    REQUIRES
        esp_event
        nvs_flash
        state_bus
)
