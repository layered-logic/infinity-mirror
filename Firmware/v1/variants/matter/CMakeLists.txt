# Layered Logic Infinity Mirror — matter variant (Pro+)
#
# Scaffold. Will REQUIRE esp-matter + connectedhomeip when real bring-up
# begins. 8MB flash target; factory partition is skipped to make room.

idf_component_register(
    SRCS "main.c" "matter_bridge.c"
    INCLUDE_DIRS "."
    REQUIRES
        # esp-matter, connectedhomeip, openthread (C6), ieee802154 (C6)
        # plus all core components
)
