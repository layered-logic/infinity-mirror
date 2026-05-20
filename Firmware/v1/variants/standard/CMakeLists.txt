# Layered Logic Infinity Mirror — standard variant (Pro)
#
# Scaffold only. Real component registration lands when core/ modules
# have implementations.

idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES
        esp_event
        state_bus
        button
        captive_dns
        led_driver
        ll_mdns
        nvs
        ota
        pattern_interp
        provisioning
        telemetry
        transport
        # auth is listed last on purpose: core/auth pulls in IDF nvs_flash,
        # and listing auth earlier puts IDF's nvs.h ahead of core/nvs/nvs.h
        # on main.c's include path — which breaks ll_nvs_init resolution.
        auth
)
