# Host-side firmware tests

Unit tests for the pure-C modules of the firmware (anything that doesn't
pull in ESP-IDF / FreeRTOS). Built with plain CMake + a host C compiler.

## Run

```
cmake -S tests -B tests/build
cmake --build tests/build
ctest --test-dir tests/build --output-on-failure
```

The executable also runs standalone (`./tests/build/ll_firmware_tests`) if
you want raw output.

## Scope

Testable on host:
- `core/button/button_logic.c` — gesture state machines + cycle helpers
  (pure C, no ESP-IDF).
- `core/led_driver/led_driver_logic.c` — GRB reorder + brightness scaling
  for WS2812 frames (pure C, no RMT).
- `core/pattern_interp/patterns.c` — 7 stateless pattern renderers + the
  registry lookup.
- `core/pattern_interp/cue_logic.c` — cue overlay state machine and
  rendering for recessed-hold / factory-reset feedback.

Not tested here (on-target only):
- `core/button/button.c` — GPIO ISR + FreeRTOS task glue
- `core/state_bus/state_bus.c` — uses esp_event loop; would need a mock
- `core/led_driver/led_driver.c` — ESP-IDF RMT TX + WS2812 encoder
- `core/pattern_interp/pattern_interp.c` — render task + esp_event glue

If the on-target coverage gap matters, the next step is vendoring Unity
and running `idf.py build` + `idf.py -T button flash monitor` with a
test-runner app under `tests/on_target/`.
