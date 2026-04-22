#pragma once

/*
 * Button module — two-button input (primary + recessed).
 *
 * Initializes GPIO ISRs + a dedicated task that runs the gesture state
 * machines and posts resolved gestures to state_bus as LL_EV_* events.
 *
 * Must be called after ll_state_bus_init(). See firmware-spec.md §4.3
 * and button-interface.md for the UX contract.
 */

void ll_button_init(void);
