#pragma once

/*
 * V1 Pro shipping PCB (ESP32-C3-MINI-1 module target).
 *
 * STUB — all pins TBD at PCB layout. Committed as a placeholder so the
 * board-header interface is exercised end-to-end even before hardware
 * exists. Fill in when layout lands.
 */

#define LL_PIN_LED_DATA        (-1)   /* TBD at layout */
#define LL_PIN_BUTTON_PRIMARY  (-1)   /* TBD */
#define LL_PIN_BUTTON_RESET    (-1)   /* TBD */
#define LL_PIN_LED_POWER_EN    (-1)   /* TBD — may exist on Pro for low-power idle */

#define LL_HAS_WIFI            1
#define LL_HAS_BLE             1
#define LL_HAS_802154          0

#define LL_PARTITION_SCHEME    "ab_with_factory"   /* 4MB, A/B + factory */

#define LL_LED_COUNT_DEFAULT   32
#define LL_LED_COLOR_ORDER     GRB

#error "prod_v1_pro.h is a layout-pending stub — finalize pins before real build"
