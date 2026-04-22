/*
 * Layered Logic Infinity Mirror — Matter variant (Pro+).
 *
 * Scaffold: blink-level entry point. This variant additionally links
 * esp-matter and exposes On/Off + Level + Color Control clusters.
 *
 * Matter bridge (cluster callbacks → state bus) lives in matter_bridge.c.
 * See ../../../../docs/firmware-spec.md §5.2.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "board.h"

void app_main(void)
{
    printf("Layered Logic Infinity Mirror — matter variant (uncertified, test VID 0xFFF1)\n");
    printf("Board: LED pin=%d, primary btn=%d, reset btn=%d\n",
           LL_PIN_LED_DATA, LL_PIN_BUTTON_PRIMARY, LL_PIN_BUTTON_RESET);
    printf("15.4 radio present: %d\n", LL_HAS_802154);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
