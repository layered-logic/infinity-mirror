#include <stdio.h>

#include "test_harness.h"

int g_tests_run = 0;
int g_tests_failed = 0;

void run_one(void (*fn)(void), const char *name)
{
    int before = g_tests_failed;
    g_tests_run++;
    printf("  %-45s ", name);
    fn();
    if (g_tests_failed == before) {
        printf("OK\n");
    }
}

/* Suite entry points — one per test file. */
void suite_auth_logic(void);
void suite_gesture_primary(void);
void suite_gesture_recessed(void);
void suite_cycle_color(void);
void suite_cycle_pattern(void);
void suite_cycle_brightness(void);
void suite_led_driver_encode(void);
void suite_ll_wifi_logic(void);
void suite_nvs_logic(void);
void suite_patterns(void);
void suite_cue(void);

int main(void)
{
    printf("== Layered Logic firmware tests ==\n\n");

    printf("[primary gesture machine]\n");
    suite_gesture_primary();

    printf("\n[recessed gesture machine]\n");
    suite_gesture_recessed();

    printf("\n[color cycle]\n");
    suite_cycle_color();

    printf("\n[pattern cycle]\n");
    suite_cycle_pattern();

    printf("\n[brightness cycle]\n");
    suite_cycle_brightness();

    printf("\n[led_driver encode]\n");
    suite_led_driver_encode();

    printf("\n[ll_wifi logic]\n");
    suite_ll_wifi_logic();

    printf("\n[nvs logic]\n");
    suite_nvs_logic();

    printf("\n[pattern renderers]\n");
    suite_patterns();

    printf("\n[cue state machine]\n");
    suite_cue();

    printf("\n[auth logic]\n");
    suite_auth_logic();

    printf("\n%d run, %d failed\n", g_tests_run, g_tests_failed);
    return g_tests_failed == 0 ? 0 : 1;
}
