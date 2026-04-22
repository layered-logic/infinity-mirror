#pragma once

/*
 * Board abstraction interface.
 *
 * Every board header (c6_devkit.h, c3_devkit.h, prod_v1_pro.h, prod_v1_pro_plus.h)
 * must define the constants below. The active board header is selected at
 * compile time via -DLL_BOARD=<name>, which the top-level CMakeLists
 * resolves to the right include.
 *
 * Don't add product/brand choices here — keep this file strictly about the
 * chip and how it's wired. See firmware-architecture-scoping.md §7.3 for the
 * list of things that live elsewhere.
 */

#if defined(LL_BOARD_C6_DEVKIT)
    #include "c6_devkit.h"
#elif defined(LL_BOARD_C3_DEVKIT)
    #include "c3_devkit.h"
#elif defined(LL_BOARD_PROD_V1_PRO)
    #include "prod_v1_pro.h"
#elif defined(LL_BOARD_PROD_V1_PRO_PLUS)
    #include "prod_v1_pro_plus.h"
#else
    #error "No LL_BOARD_* defined. Pass -DLL_BOARD_C6_DEVKIT (or equivalent) at build time."
#endif

/* Required symbols — every board header must define these. */
#ifndef LL_PIN_LED_DATA
    #error "board header missing LL_PIN_LED_DATA"
#endif
#ifndef LL_PIN_BUTTON_PRIMARY
    #error "board header missing LL_PIN_BUTTON_PRIMARY"
#endif
#ifndef LL_HAS_WIFI
    #error "board header missing LL_HAS_WIFI"
#endif
#ifndef LL_HAS_BLE
    #error "board header missing LL_HAS_BLE"
#endif
#ifndef LL_HAS_802154
    #error "board header missing LL_HAS_802154"
#endif
#ifndef LL_LED_COUNT_DEFAULT
    #error "board header missing LL_LED_COUNT_DEFAULT"
#endif
