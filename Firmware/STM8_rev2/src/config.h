/*
 * config.h -- build-time configuration.
 *
 * Everything here can be overridden from the command line, e.g.
 *     make LEDS=60
 *     make LEDS=60 IR=0
 *     make BRINGUP=1
 */
#ifndef CONFIG_H
#define CONFIG_H

/* ---- LED strip --------------------------------------------------------- *
 * Physical count for the fixture this board is fitted to.  32 is the 150 mm
 * (6"x6") build; the rev-2 board is specced to <= 100.  This is NOT a user
 * setting -- it must match the strip actually soldered to J7, because every
 * current budget below is derived from it.                                 */
#ifndef NUM_LEDS
#define NUM_LEDS 32
#endif

/* Hard bounds, both of which are real:
 *  - pattern_spinner() divides by NUM_LEDS/4, which is 0 below 4 LEDs;
 *  - pattern_scanner() reduces animStep modulo NUM_LEDS*2 into a uint8_t,
 *    so the sweep silently truncates above 127.
 * The SKU is specced to <= 100 anyway (REWORK_SPEC decision 9). */
#if (NUM_LEDS < 4) || (NUM_LEDS > 100)
#error "NUM_LEDS must be 4..100 -- see the note above, and the SKU spec."
#endif

/* ---- Current budget ---------------------------------------------------- *
 * One WS2812B at full white draws ~60 mA, so the strip's worst case is
 * NUM_LEDS * 60 mA and the brightness ceiling for a given supply budget is
 *      cap = 255 * budget_mA / (NUM_LEDS * 60)
 * clamped to [MIN_BRIGHTNESS, 255].  The compiler does this, so changing
 * LEDS re-derives the caps instead of leaving three stale magic numbers.
 *
 * Budgets by advertised USB-C source current.  The top tier is 2400 mA, not
 * 3000: REWORK_SPEC decision 9 holds this SKU to the C3's proven 2.4 A ABL
 * bound (JST XH is a 3 A connector and J1 is rated 3 A -- 2.4 A is the
 * deliberate margin, don't "fix" it upward).                               */
#define BUDGET_MA_DEFAULT  700      /* Rp default (500-900 mA advertised)   */
#define BUDGET_MA_1A5     1200
#define BUDGET_MA_3A0     2400

/* ---- CC decode --------------------------------------------------------- *
 * VDD is raw VBUS, so the ADC is RATIOMETRIC: compare fractions of full
 * scale, never absolute volts.  With Rd = 5.1k the CC node sits at 0.41 V
 * (default USB), 0.92 V (1.5 A) or 1.68 V (3.0 A); across VDD 4.75-5.5 V
 * those land at codes 76-88, 171-198 and 313-362.  The gaps are wide; these
 * thresholds sit mid-gap.                                                  */
#define CC_TH_1A5   130
#define CC_TH_3A0   255
#define CC_POLL_MS  1000            /* the cable can be swapped any time    */

/* ---- User-facing behaviour (product contract -- see spec.md) ------------ */
#define NUM_COLORS          13
#define NUM_PATTERNS        7
#define DEFAULT_BRIGHTNESS  128     /* ~50%                                 */
#define MIN_BRIGHTNESS      25      /* ~10%, floor before wrap              */
#define BRIGHTNESS_STEP     25
#define WHITE_DIM_FACTOR    217     /* 85%, solid-on-white only             */

/* ---- Timing (real milliseconds -- see the timebase note in main.c) ------ */
#define DEBOUNCE_MS         20
#define DOUBLE_CLICK_MS    200
#define LONG_PRESS_MS      600
#define PATTERN_STEP_MS     50      /* 20 Hz reference tick                 */

/* ---- Optional features ------------------------------------------------- */
#ifndef ENABLE_UART
#define ENABLE_UART 1               /* TP1, 115200 8N1, transmit only       */
#endif
/* Default OFF: J8 is DNP and no board currently has a receiver fitted.
 * Build with IR=1 once one is.  Leaving it off also keeps the only
 * interrupt in the design out of a board that cannot use it. */
#ifndef ENABLE_IR
#define ENABLE_IR   0
#endif
#ifndef ENABLE_BRINGUP
#define ENABLE_BRINGUP 0            /* 1 = board bring-up, ignores buttons  */
#endif

/* NEC command bytes.  PLACEHOLDERS -- they are remote-specific.  Every
 * received frame is printed on TP1 as addr/cmd hex, so capture your remote
 * first (see README "Teaching it a remote") and then correct these.  Left
 * as distinct dummies so an unknown remote does nothing rather than
 * firing the wrong action.                                                 */
#define IR_CMD_POWER     0x45
#define IR_CMD_COLOR     0x46
#define IR_CMD_PATTERN   0x47
#define IR_CMD_BRIGHT_UP 0x40
#define IR_CMD_BRIGHT_DN 0x19

/* ---- Derived: brightness ceilings -------------------------------------- */
#define _CAP_RAW(mA)  ((uint16_t)(((uint32_t)(mA) * 255UL) / ((uint32_t)NUM_LEDS * 60UL)))
#define _CAP(mA)      ((uint8_t)( _CAP_RAW(mA) > 255 ? 255 \
                                : _CAP_RAW(mA) < MIN_BRIGHTNESS ? MIN_BRIGHTNESS \
                                : _CAP_RAW(mA) ))
#define BRIGHT_CAP_DEFAULT  _CAP(BUDGET_MA_DEFAULT)
#define BRIGHT_CAP_1A5      _CAP(BUDGET_MA_1A5)
#define BRIGHT_CAP_3A0      _CAP(BUDGET_MA_3A0)

/* At 32 LEDs this reproduces the shipped 90/160/255 (as 92/159/255).  Past
 * ~55 LEDs the default-USB cap hits the MIN_BRIGHTNESS floor, i.e. a long
 * strip on a dumb USB port is honestly beyond budget and the floor is the
 * only thing keeping it visible.  Plug it into a real 3 A charger.         */

#endif /* CONFIG_H */
