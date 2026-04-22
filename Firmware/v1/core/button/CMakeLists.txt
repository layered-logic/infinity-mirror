# core/button — two-button input (primary + recessed).

idf_component_register(
    SRCS
        "button.c"
        "button_logic.c"
    INCLUDE_DIRS "."
    REQUIRES
        driver
        esp_timer
        state_bus
)
