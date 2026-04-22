# core/led_driver — WS2812B output over RMT.

idf_component_register(
    SRCS
        "led_driver.c"
        "led_driver_logic.c"
    INCLUDE_DIRS "."
    REQUIRES
        driver
        esp_driver_rmt
)
