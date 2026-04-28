#pragma once

/*
 * ESP32-C3-DevKitM-1
 *
 * V0 prototype hardware. Kept in the scaffold for regression testing on
 * the pre-V1 silicon. No recessed button on this physical board — provisioning
 * bring-up via app triggering over serial, or bench-test only.
 */

#define LL_PIN_LED_DATA        8
#define LL_PIN_BUTTON_PRIMARY  9
#define LL_PIN_BUTTON_RESET    (-1)   /* not present; button module skips registration */
#define LL_PIN_LED_POWER_EN    (-1)

#define LL_HAS_WIFI            1
#define LL_HAS_BLE             1
#define LL_HAS_802154          0      /* C3 has no 15.4 radio */

#define LL_PARTITION_SCHEME    "ab_with_factory"   /* 4MB, A/B + factory */

/* The V0 C3 prototype is the 12×12 / 66-LED test rig (per
 * project_quarter_spring2026 memory: "12×12 (66 LEDs) is a firmware test
 * prototype"). Production 6×6 / 32-LED ships on prod_v1_pro.h. */
#define LL_LED_COUNT_DEFAULT   66
#define LL_LED_COLOR_ORDER     GRB
