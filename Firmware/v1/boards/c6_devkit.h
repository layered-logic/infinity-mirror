#pragma once

/*
 * ESP32-C6-DevKitC-1
 *
 * Target for both V1 demo units as of Apr 2026. Used for:
 *   - standard variant bring-up (Pro behavior on C6 silicon)
 *   - matter variant bring-up (Pro+ uncertified Matter)
 */

/* Pin assignments. TBD entries finalized at first hardware bring-up. */
#define LL_PIN_LED_DATA        8      /* WS2812B data line */
#define LL_PIN_BUTTON_PRIMARY  9      /* exposed momentary button, active-low */
#define LL_PIN_BUTTON_RESET    10     /* recessed pinhole, active-low — TBD, check silk */
#define LL_PIN_LED_POWER_EN    (-1)   /* unused on devkit */

/* Radio capability. */
#define LL_HAS_WIFI            1
#define LL_HAS_BLE             1
#define LL_HAS_802154          1      /* Thread + Zigbee radio present */

/* Flash layout. */
#define LL_PARTITION_SCHEME    "ab_no_factory"   /* 8MB, A/B OTA, no factory */

/* LED defaults. */
#define LL_LED_COUNT_DEFAULT   32     /* 6x6 shipping config */
#define LL_LED_COLOR_ORDER     GRB
