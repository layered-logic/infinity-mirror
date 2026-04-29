# core/ll_mdns — Layered Logic mDNS service publisher.
#
# Wraps the espressif/mdns managed component (declared in
# idf_component.yml). Owns the _layeredlogic._tcp.local service
# advertisement and its TXT records.
#
# Folder named ll_mdns/ (not mdns/) to avoid an IDF component-name
# collision: the espressif/mdns managed component registers as `mdns`
# in the build graph, and a same-named local component triggers
# CMake's "Target __idf_mdns links to itself" error.

idf_component_register(
    SRCS "ll_mdns.c"
    INCLUDE_DIRS "."
    REQUIRES
        esp_app_format
        esp_event
        esp_netif
        esp_wifi
        mdns
        state_bus
)
