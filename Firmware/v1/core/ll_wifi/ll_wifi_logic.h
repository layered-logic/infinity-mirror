#pragma once

/*
 * Pure-C list operations for the multi-network Wi-Fi store.
 *
 * Split from ll_wifi.c so it builds on the host for unit tests. ll_wifi.c
 * owns the ESP-IDF NVS plumbing and the migration shim from esp_wifi's
 * saved cred; everything that mutates the in-memory list lives here.
 *
 * Spec: docs/multi-network-design.md §5 (storage layout) and §6.3 (priority).
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define LL_WIFI_SSID_MAX     32u   /* IEEE 802.11 caps SSID at 32 octets */
#define LL_WIFI_PASSWORD_MAX 64u   /* WPA2-PSK caps PSK at 64 octets */
#define LL_WIFI_LIST_MAX     4u    /* §5.2: realistic personal-use ceiling */
#define LL_WIFI_ACTIVE_NONE  0xFFu

/*
 * On-flash blob form for one saved network. Field order matches
 * docs/multi-network-design.md §5.1 verbatim — that doc is the contract.
 *
 *   ssid:          32 + null
 *   password:      64 + null; empty == open network
 *   last_used_us:  esp_timer_get_time() at last successful connect
 */
typedef struct __attribute__((packed)) {
    char    ssid[LL_WIFI_SSID_MAX + 1];
    char    password[LL_WIFI_PASSWORD_MAX + 1];
    int64_t last_used_us;
} ll_wifi_entry_t;

/*
 * In-memory list. Mirrors the on-flash layout but kept separate so the
 * add/remove logic works on plain structs without dragging NVS into the
 * host-test surface.
 *
 * active_idx == LL_WIFI_ACTIVE_NONE when nothing is connected, else the
 * index into entries[] of the currently-connected network.
 */
typedef struct {
    uint8_t         count;
    uint8_t         active_idx;
    ll_wifi_entry_t entries[LL_WIFI_LIST_MAX];
} ll_wifi_list_t;

typedef enum {
    LL_WIFI_ADD_OK_INSERTED = 0,  /* new ssid, appended */
    LL_WIFI_ADD_OK_UPDATED  = 1,  /* ssid existed, password rewritten */
    LL_WIFI_ADD_ERR_FULL    = 2,  /* count == LL_WIFI_LIST_MAX, ssid not present */
    LL_WIFI_ADD_ERR_INVALID = 3,  /* ssid empty/oversized or password oversized */
} ll_wifi_add_result_t;

typedef enum {
    LL_WIFI_REMOVE_OK        = 0,
    LL_WIFI_REMOVE_NOT_FOUND = 1,
} ll_wifi_remove_result_t;

/* Initialize an empty list (count=0, active_idx=NONE, entries zeroed). */
void ll_wifi_list_init(ll_wifi_list_t *list);

/*
 * Find the entry index for an ssid. Returns -1 if not present. Comparison
 * is exact, case-sensitive, capped at LL_WIFI_SSID_MAX bytes (so a
 * non-null-terminated stored ssid still matches a well-formed input).
 */
int ll_wifi_list_find(const ll_wifi_list_t *list, const char *ssid);

/*
 * Insert or replace. If ssid already exists, password is overwritten and
 * last_used_us is preserved. Otherwise appends with last_used_us = 0 so
 * the new entry sorts last under "last-used wins" until first connect.
 *
 * On any non-OK result, the list is unmodified.
 */
ll_wifi_add_result_t ll_wifi_list_add(ll_wifi_list_t *list,
                                       const char *ssid,
                                       const char *password);

/*
 * Remove by ssid. Compacts entries[] and adjusts active_idx:
 *   - removed slot was active        → active_idx → ACTIVE_NONE
 *   - removed slot precedes active   → active_idx decremented
 *   - removed slot follows active    → active_idx unchanged
 */
ll_wifi_remove_result_t ll_wifi_list_remove(ll_wifi_list_t *list,
                                             const char *ssid);

/*
 * Pick the next candidate index to attempt a connection on, given a mask
 * of indices that have failed in the current boot session.
 *
 * Selection rule (§6.3): highest last_used_us among entries not in the
 * recently_failed mask. On ties (e.g., multiple never-connected entries
 * with last_used_us == 0), the lowest index wins — deterministic and
 * doesn't depend on insertion order beyond the list itself.
 *
 * Returns -1 if the list is empty or every entry is masked.
 *
 * recently_failed_mask is a bitfield with bit i set when entries[i] has
 * already failed this session. Bits beyond `count` are ignored.
 */
int ll_wifi_list_pick_next(const ll_wifi_list_t *list,
                            uint8_t recently_failed_mask);

/*
 * Force null-termination at the buffer boundary on both strings. Returns
 * true if the resulting entry has a non-empty SSID (i.e., the slot looks
 * populated); false means the slot is junk and the caller should drop it.
 */
bool ll_wifi_entry_sanitize(ll_wifi_entry_t *entry);
