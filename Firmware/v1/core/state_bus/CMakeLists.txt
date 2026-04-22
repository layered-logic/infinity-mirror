# core/state_bus — unified state bus component.

idf_component_register(
    SRCS "state_bus.c"
    INCLUDE_DIRS "."
    REQUIRES esp_event
)
