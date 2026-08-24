/*
 * Host test for src/button_logic.h -- the real header, not a copy.
 *
 *     cc -O2 -Wall -Wextra -o build/test_button tools/test_button_logic.c && ./build/test_button
 *
 * Drives the machine one millisecond at a time, the way the firmware loop
 * does, and asserts the gesture contract from docs/button-interface.md.
 * The case that earned this file is LONG_ONCE: the old firmware re-fired
 * BTN_LONG for as long as the button was held, and every BTN_LONG writes
 * four EEPROM bytes.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "../src/config.h"
#include "../src/button_logic.h"

static int failures = 0;

/* --- a tiny scripted driver -------------------------------------------- */
typedef struct { ButtonSM sm; int counts[5]; } Rig;

static void rig_init(Rig *r) { button_reset(&r->sm); memset(r->counts, 0, sizeof r->counts); }

/* Advance `ms` milliseconds with the button in state `pressed`, ticking at
 * 1 ms like the firmware's fastest realistic service rate. */
static void hold(Rig *r, uint16_t ms, uint8_t pressed) {
    for (uint16_t i = 0; i < ms; i++)
        r->counts[button_service(&r->sm, 1, pressed)]++;
}

static void click(Rig *r, uint16_t downMs, uint16_t upMs) {
    hold(r, downMs, 1);
    hold(r, upMs, 0);
}

static const char *ev_name(int e) {
    static const char *n[] = {"NONE", "SINGLE", "DOUBLE", "TRIPLE", "LONG"};
    return n[e];
}

static void expect(const char *test, Rig *r, int single, int dbl, int triple, int lng) {
    int want[5] = {0, single, dbl, triple, lng};
    int ok = 1;
    for (int e = 1; e <= 4; e++) if (r->counts[e] != want[e]) ok = 0;
    if (ok) { printf("  PASS  %s\n", test); return; }
    failures++;
    printf("  FAIL  %s\n", test);
    for (int e = 1; e <= 4; e++)
        printf("          %-7s want %d got %d\n", ev_name(e), want[e], r->counts[e]);
}

int main(void) {
    Rig r;

    printf("button_logic: DEBOUNCE=%d DOUBLE_CLICK=%d LONG_PRESS=%d\n\n",
           DEBOUNCE_MS, DOUBLE_CLICK_MS, LONG_PRESS_MS);

    /* --- the four gestures --------------------------------------------- */
    rig_init(&r);
    click(&r, 60, 40);
    hold(&r, DOUBLE_CLICK_MS + 50, 0);
    expect("single click -> SINGLE", &r, 1, 0, 0, 0);

    rig_init(&r);
    click(&r, 60, 40);
    click(&r, 60, 40);
    hold(&r, DOUBLE_CLICK_MS + 50, 0);
    expect("double click -> DOUBLE", &r, 0, 1, 0, 0);

    rig_init(&r);
    click(&r, 60, 40);
    click(&r, 60, 40);
    click(&r, 60, 40);
    hold(&r, DOUBLE_CLICK_MS + 50, 0);
    expect("triple click -> TRIPLE", &r, 0, 0, 1, 0);

    rig_init(&r);
    click(&r, LONG_PRESS_MS + 100, 100);
    hold(&r, DOUBLE_CLICK_MS + 50, 0);
    expect("long press -> LONG", &r, 0, 0, 0, 1);

    /* --- LONG_ONCE: the regression this file exists for ------------------ */
    rig_init(&r);
    click(&r, 5000, 100);                  /* five full seconds held down   */
    hold(&r, DOUBLE_CLICK_MS + 50, 0);
    expect("5 s hold fires LONG exactly once", &r, 0, 0, 0, 1);

    /* A long press must not also register as a click on release. */
    rig_init(&r);
    click(&r, LONG_PRESS_MS + 100, 100);
    hold(&r, DOUBLE_CLICK_MS + 50, 0);
    click(&r, 60, 40);
    hold(&r, DOUBLE_CLICK_MS + 50, 0);
    expect("long then single -> LONG + SINGLE", &r, 1, 0, 0, 1);

    /* --- debounce ------------------------------------------------------- */
    rig_init(&r);
    for (int i = 0; i < 6; i++) click(&r, 2, 2);   /* contact chatter, < DEBOUNCE_MS */
    hold(&r, DOUBLE_CLICK_MS + 50, 0);
    expect("sub-debounce chatter is ignored", &r, 0, 0, 0, 0);

    /* Bounce on release must not split one click into two. */
    rig_init(&r);
    hold(&r, 60, 1);
    hold(&r, 3, 0); hold(&r, 3, 1);        /* release bounce */
    hold(&r, 60, 1);
    hold(&r, DEBOUNCE_MS + 40, 0);
    hold(&r, DOUBLE_CLICK_MS + 50, 0);
    expect("release bounce stays one click", &r, 1, 0, 0, 0);

    /* --- gaps ----------------------------------------------------------- */
    rig_init(&r);
    click(&r, 60, 40);
    hold(&r, DOUBLE_CLICK_MS + 50, 0);     /* window closes between them */
    click(&r, 60, 40);
    hold(&r, DOUBLE_CLICK_MS + 50, 0);
    expect("two slow clicks -> two SINGLE", &r, 2, 0, 0, 0);

    /* --- coarse dt: the loop is slower during a frame ------------------- */
    rig_init(&r);
    for (int i = 0; i < 4; i++) r.counts[button_service(&r.sm, 20, 1)]++;   /* 80 ms down */
    for (int i = 0; i < 4; i++) r.counts[button_service(&r.sm, 20, 0)]++;   /* 80 ms up   */
    for (int i = 0; i < 20; i++) r.counts[button_service(&r.sm, 20, 0)]++;
    expect("20 ms service granularity still gives SINGLE", &r, 1, 0, 0, 0);

    printf("\n%s (%d failure%s)\n", failures ? "FAILED" : "ALL PASS",
           failures, failures == 1 ? "" : "s");
    return failures ? 1 : 0;
}
