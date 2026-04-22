/*
 * Layered Logic Infinity Mirror — Standard variant (Pro).
 *
 * Scaffold: blink-level entry point. Real bring-up lands incrementally
 * in core/. See ../../README.md and ../../../../docs/firmware-spec.md.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "board.h"

void app_main(void)
{
    printf("Layered Logic Infinity Mirror — standard variant\n");
    printf("Board: LED pin=%d, primary btn=%d, reset btn=%d\n",
           LL_PIN_LED_DATA, LL_PIN_BUTTON_PRIMARY, LL_PIN_BUTTON_RESET);
    printf("Default LED count: %d\n", LL_LED_COUNT_DEFAULT);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
