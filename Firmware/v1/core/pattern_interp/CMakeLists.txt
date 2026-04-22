# core/pattern_interp — render loop, pattern bank, cue overlays.

idf_component_register(
    SRCS
        "pattern_interp.c"
        "patterns.c"
        "cue_logic.c"
    INCLUDE_DIRS "."
    REQUIRES
        esp_event
        esp_timer
        state_bus
        led_driver
)
