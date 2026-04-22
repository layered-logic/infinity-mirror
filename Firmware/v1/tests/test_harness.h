#pragma once

/*
 * Tiny test harness — no third-party deps. If we hit ~50 tests or start
 * wanting parameterized / table-driven tests, swap to Unity or CMocka.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

extern int g_tests_run;
extern int g_tests_failed;

void run_one(void (*fn)(void), const char *name);

#define RUN(fn) run_one(fn, #fn)

#define ASSERT(cond) do {                                         \
    if (!(cond)) {                                                \
        printf("FAIL\n    %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        g_tests_failed++;                                         \
        return;                                                   \
    }                                                             \
} while (0)

#define ASSERT_EQ(a, b) do {                                      \
    long _a = (long)(a), _b = (long)(b);                          \
    if (_a != _b) {                                               \
        printf("FAIL\n    %s:%d: %s = %ld, expected %s = %ld\n",  \
               __FILE__, __LINE__, #a, _a, #b, _b);               \
        g_tests_failed++;                                         \
        return;                                                   \
    }                                                             \
} while (0)

#define ASSERT_EQ_HEX(a, b) do {                                  \
    unsigned long _a = (unsigned long)(a), _b = (unsigned long)(b); \
    if (_a != _b) {                                               \
        printf("FAIL\n    %s:%d: %s = 0x%06lX, expected %s = 0x%06lX\n", \
               __FILE__, __LINE__, #a, _a, #b, _b);               \
        g_tests_failed++;                                         \
        return;                                                   \
    }                                                             \
} while (0)

#define ASSERT_STR_EQ(a, b) do {                                  \
    const char *_a = (a); const char *_b = (b);                   \
    if (strcmp(_a, _b) != 0) {                                    \
        printf("FAIL\n    %s:%d: %s = \"%s\", expected %s = \"%s\"\n", \
               __FILE__, __LINE__, #a, _a, #b, _b);               \
        g_tests_failed++;                                         \
        return;                                                   \
    }                                                             \
} while (0)
