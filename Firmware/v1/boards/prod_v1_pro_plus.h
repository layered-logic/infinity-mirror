#pragma once

/*
 * V1 Pro+ shipping PCB (ESP32-C6 module target).
 *
 * STUB — all pins TBD at PCB layout. Pro+ carries Matter; flash budget
 * sized to 8MB accordingly.
 */

#define LL_PIN_LED_DATA        (-1)   /* TBD at layout */
#define LL_PIN_BUTTON_PRIMARY  (-1)   /* TBD */
#define LL_PIN_BUTTON_RESET    (-1)   /* TBD */
#define LL_PIN_LED_POWER_EN    (-1)   /* TBD */

#define LL_HAS_WIFI            1
#define LL_HAS_BLE             1
#define LL_HAS_802154          1      /* Thread/Zigbee for Matter-over-Thread */

#define LL_PARTITION_SCHEME    "ab_no_factory"   /* 8MB, A/B OTA, no factory */

#define LL_LED_COUNT_DEFAULT   32
#define LL_LED_COLOR_ORDER     GRB

#error "prod_v1_pro_plus.h is a layout-pending stub — finalize pins before real build"
