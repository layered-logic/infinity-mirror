#pragma once

/*
 * Pattern interpreter — subscribes to state_bus, renders a pattern frame
 * every ~16ms, and pushes it to led_driver. Also applies transient cue
 * overlays (recessed-hold blue blink, factory-reset red flash / green
 * hold) on top of the base pattern.
 *
 * Owns the LED output end-to-end: init brings up led_driver for you, so
 * callers don't need to touch it directly.
 *
 * Depends on: state_bus (subscribed), button (indirectly, via state_bus
 * events), led_driver (owned), board.h (LED count default).
 */

/*
 * Start the render task (core 1) and register event subscriptions on the
 * state_bus loop. Call after ll_state_bus_init(). Blocks briefly during
 * led_driver init; otherwise returns immediately and rendering continues
 * in the background.
 */
void ll_pattern_interp_init(void);
