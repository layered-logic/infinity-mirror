cmake_minimum_required(VERSION 3.16)

# Host-side unit tests for pure-C firmware modules. Builds without
# ESP-IDF — just needs gcc/clang/MSVC.
#
# Usage:
#   cmake -S tests -B tests/build
#   cmake --build tests/build
#   ./tests/build/ll_firmware_tests         (Linux/macOS/msys)
#   .\tests\build\Debug\ll_firmware_tests   (MSVC)
#
# Or via ctest:
#   ctest --test-dir tests/build --output-on-failure

project(ll_firmware_tests C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(ll_firmware_tests
    test_main.c
    core/button/test_gesture.c
    core/button/test_cycle.c
    core/led_driver/test_encode.c
    core/nvs/test_nvs_logic.c
    core/pattern_interp/test_patterns.c
    core/pattern_interp/test_cue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/button/button_logic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/led_driver/led_driver_logic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/nvs/nvs_logic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/pattern_interp/patterns.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/pattern_interp/cue_logic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/state_bus/state_bus_defaults.c
)

target_include_directories(ll_firmware_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/button
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/led_driver
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/nvs
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/pattern_interp
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/state_bus
)

enable_testing()
add_test(NAME ll_firmware_tests COMMAND ll_firmware_tests)
