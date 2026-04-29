cmake_minimum_required(VERSION 3.16)

# Layered Logic Infinity Mirror — top-level build.
#
# Variant selection: set LL_VARIANT to "standard" or "matter".
# Board selection:   set LL_BOARD   to "c6_devkit" | "c3_devkit" | "prod_v1_pro" | "prod_v1_pro_plus".
#
# Example:
#   idf.py set-target esp32c6 -DLL_VARIANT=standard -DLL_BOARD=c6_devkit build
#
# See README.md for the full build-target matrix.

if(NOT DEFINED LL_VARIANT)
    set(LL_VARIANT "standard")
endif()

if(NOT DEFINED LL_BOARD)
    set(LL_BOARD "c6_devkit")
endif()

# Map LL_BOARD to the preprocessor symbol the board.h dispatcher expects.
string(TOUPPER ${LL_BOARD} LL_BOARD_UPPER)
add_compile_definitions(LL_BOARD_${LL_BOARD_UPPER})

# Expose LL_VARIANT as a string macro for runtime identification (mdns
# TXT records, future telemetry beacons, advisor demo logs, etc.).
# Backslash-escaped quotes survive cmd.exe / shell intermediation on
# Windows hosts.
add_compile_definitions(LL_VARIANT_NAME=\"${LL_VARIANT}\")

# Include the board headers on the global include path.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/boards)

# Point ESP-IDF at the variant's main/ directory.
set(EXTRA_COMPONENT_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/variants/${LL_VARIANT}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(layered_logic_mirror_${LL_VARIANT})
