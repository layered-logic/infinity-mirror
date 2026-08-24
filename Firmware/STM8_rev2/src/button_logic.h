/*
 * button_logic.h -- the single-button gesture machine, as pure logic.
 *
 * No registers, no globals, no hardware: it takes "how many milliseconds
 * since I last called you" and "is the button down right now", and returns
 * an event.  Same split as the ESP32 firmware's core/button/button_logic.c,
 * and for the same reason -- this is the part worth testing on a host,
 * where a full gesture costs microseconds instead of a finger.
 *
 * tools/test_button_logic.c compiles THIS FILE with gcc and drives it.
 * Header-only and static, so the STM8 build stays a single translation unit
 * (which is what keeps the interrupt vector table correct -- see stm8s_it.h).
 *
 * The gesture contract (docs/button-interface.md):
 *   single  -- on, or advance colour
 *   double  -- advance pattern
 *   triple  -- brightness down one step, wrapping
 *   long    -- off + save, fired DURING the hold, exactly once per hold
 *
 * Requires DEBOUNCE_MS, DOUBLE_CLICK_MS and LONG_PRESS_MS from config.h.
 */
#ifndef BUTTON_LOGIC_H
#define BUTTON_LOGIC_H

typedef enum { BTN_NONE = 0, BTN_SINGLE, BTN_DOUBLE, BTN_TRIPLE, BTN_LONG } ButtonEvent;

typedef enum {
  BS_IDLE = 0,        /* waiting for a press                    */
  BS_DEBOUNCE_PRESS,  /* press seen, waiting for it to hold up  */
  BS_PRESSED,         /* down; long-press timer running         */
  BS_DEBOUNCE_REL,    /* release seen, waiting for it to settle */
  BS_WAIT_MULTI       /* released, waiting out the multi-click window */
} ButtonState;

typedef struct {
  ButtonState state;
  uint8_t     clickCount;
  uint16_t    debounceMs;
  uint16_t    holdMs;
  uint16_t    multiMs;
  uint8_t     wasLong;    /* this hold has already fired its long press */
} ButtonSM;

static void button_reset(ButtonSM *b) {
  b->state = BS_IDLE; b->clickCount = 0;
  b->debounceMs = 0;  b->holdMs = 0; b->multiMs = 0; b->wasLong = 0;
}

/* dt = milliseconds since the previous call; pressed = 1 while the button
 * is down.  Call as fast as the loop spins -- dt is usually 0, which is
 * exactly what gives the debounce something to filter. */
static ButtonEvent button_service(ButtonSM *b, uint16_t dt, uint8_t pressed) {
  ButtonEvent event = BTN_NONE;

  switch (b->state) {
    case BS_IDLE:
      if (pressed) { b->state = BS_DEBOUNCE_PRESS; b->debounceMs = 0; }
      break;

    case BS_DEBOUNCE_PRESS:
      b->debounceMs = (uint16_t)(b->debounceMs + dt);
      if (!pressed) {
        b->state = BS_IDLE;                              /* bounce */
      } else if (b->debounceMs >= DEBOUNCE_MS) {
        b->state = BS_PRESSED; b->holdMs = 0; b->wasLong = 0;
      }
      break;

    case BS_PRESSED:
      b->holdMs = (uint16_t)(b->holdMs + dt);
      if (!pressed) {
        b->state = BS_DEBOUNCE_REL; b->debounceMs = 0;
      } else if (!b->wasLong && b->holdMs >= LONG_PRESS_MS) {
        /* Fires during the hold, not on release: the light going out while
         * the finger is still down is the feedback.
         *
         * The !wasLong guard is load-bearing.  Without it the hold re-enters
         * this state from BS_DEBOUNCE_REL with holdMs still past the
         * threshold and fires again every couple of iterations -- and every
         * BTN_LONG writes four EEPROM bytes.  That is what the old firmware
         * did, which is why a one-second hold cost tens of write cycles on
         * a 100 k-cycle part.  wasLong clears on the next press. */
        event = BTN_LONG; b->clickCount = 0; b->wasLong = 1;
        b->state = BS_DEBOUNCE_REL; b->debounceMs = 0;
      }
      break;

    case BS_DEBOUNCE_REL:
      b->debounceMs = (uint16_t)(b->debounceMs + dt);
      if (pressed) {
        b->state = BS_PRESSED;      /* still down; holdMs deliberately kept */
      } else if (b->debounceMs >= DEBOUNCE_MS) {
        if (b->wasLong) {
          b->state = BS_IDLE;
        } else {
          b->clickCount++;
          if (b->clickCount >= 3) {
            /* Three fires immediately -- there is no fourth gesture to
             * disambiguate against, so waiting out the window would only
             * add latency. */
            event = BTN_TRIPLE; b->clickCount = 0; b->state = BS_IDLE;
          } else {
            b->state = BS_WAIT_MULTI; b->multiMs = 0;
          }
        }
      }
      break;

    case BS_WAIT_MULTI:
      b->multiMs = (uint16_t)(b->multiMs + dt);
      if (pressed) {
        b->state = BS_DEBOUNCE_PRESS; b->debounceMs = 0;
      } else if (b->multiMs >= DOUBLE_CLICK_MS) {
        if      (b->clickCount == 1) event = BTN_SINGLE;
        else if (b->clickCount == 2) event = BTN_DOUBLE;
        b->clickCount = 0;
        b->state = BS_IDLE;
      }
      break;

    default:
      button_reset(b);
      break;
  }
  return event;
}

#endif /* BUTTON_LOGIC_H */
