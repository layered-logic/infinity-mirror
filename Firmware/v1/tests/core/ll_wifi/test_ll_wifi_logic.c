#include <string.h>

#include "ll_wifi_logic.h"
#include "test_harness.h"

/*
 * ll_wifi_logic tests — list ops on the in-memory wifi_list_t.
 *
 * These tests exercise ll_wifi_logic.c on the host, with zero ESP-IDF
 * deps. NVS persistence in ll_wifi.c (open/get_blob/set_blob/migrate)
 * is target-only and not covered here.
 */

/* ---- Helpers ------------------------------------------------------------- */

static void seed(ll_wifi_list_t *list, const char *ssid, const char *password,
                 int64_t last_used_us)
{
    ll_wifi_add_result_t r = ll_wifi_list_add(list, ssid, password);
    /* Keep this helper trivial — tests that care about add semantics call
     * add directly. Anything we seed should fit. */
    (void)r;
    int idx = ll_wifi_list_find(list, ssid);
    if (idx >= 0) {
        list->entries[idx].last_used_us = last_used_us;
    }
}

/* ---- Sizing & init ------------------------------------------------------- */

static void entry_size_locked(void)
{
    /* Wire-size lock: ssid(33) + password(65) + last_used_us(8) = 106.
     * If this breaks, the on-flash blob shape changed — bump the load
     * path and provide a migration story before relaxing this. */
    ASSERT_EQ(sizeof(ll_wifi_entry_t), 106);
}

static void init_starts_empty(void)
{
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    ASSERT_EQ(list.count, 0);
    ASSERT_EQ(list.active_idx, LL_WIFI_ACTIVE_NONE);
}

/* ---- Find ---------------------------------------------------------------- */

static void find_on_empty_returns_minus_one(void)
{
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    ASSERT_EQ(ll_wifi_list_find(&list, "Anything"), -1);
}

static void find_returns_index_when_present(void)
{
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    seed(&list, "HomeWiFi", "p1", 100);
    seed(&list, "OfficeWiFi", "p2", 200);
    ASSERT_EQ(ll_wifi_list_find(&list, "HomeWiFi"), 0);
    ASSERT_EQ(ll_wifi_list_find(&list, "OfficeWiFi"), 1);
}

static void find_is_case_sensitive(void)
{
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    seed(&list, "HomeWiFi", "p1", 100);
    ASSERT_EQ(ll_wifi_list_find(&list, "homewifi"), -1);
}

static void find_handles_null(void)
{
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    ASSERT_EQ(ll_wifi_list_find(NULL, "x"), -1);
    ASSERT_EQ(ll_wifi_list_find(&list, NULL), -1);
}

/* ---- Add ----------------------------------------------------------------- */

static void add_to_empty_inserts(void)
{
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    ll_wifi_add_result_t r = ll_wifi_list_add(&list, "HomeWiFi", "secret");
    ASSERT_EQ(r, LL_WIFI_ADD_OK_INSERTED);
    ASSERT_EQ(list.count, 1);
    ASSERT_STR_EQ(list.entries[0].ssid, "HomeWiFi");
    ASSERT_STR_EQ(list.entries[0].password, "secret");
    ASSERT_EQ((long)list.entries[0].last_used_us, 0);
}

static void add_to_full_returns_full(void)
{
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    seed(&list, "n0", "p", 0);
    seed(&list, "n1", "p", 0);
    seed(&list, "n2", "p", 0);
    seed(&list, "n3", "p", 0);
    ASSERT_EQ(list.count, LL_WIFI_LIST_MAX);

    ll_wifi_add_result_t r = ll_wifi_list_add(&list, "n4", "p");
    ASSERT_EQ(r, LL_WIFI_ADD_ERR_FULL);
    ASSERT_EQ(list.count, LL_WIFI_LIST_MAX);
}

static void add_existing_ssid_updates_password(void)
{
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    seed(&list, "HomeWiFi", "old_password", 12345);

    ll_wifi_add_result_t r = ll_wifi_list_add(&list, "HomeWiFi", "new_password");
    ASSERT_EQ(r, LL_WIFI_ADD_OK_UPDATED);
    ASSERT_EQ(list.count, 1);
    ASSERT_STR_EQ(list.entries[0].password, "new_password");
    /* last_used_us must survive a password update. */
    ASSERT_EQ((long)list.entries[0].last_used_us, 12345);
}

static void add_empty_ssid_rejected(void)
{
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    ll_wifi_add_result_t r = ll_wifi_list_add(&list, "", "p");
    ASSERT_EQ(r, LL_WIFI_ADD_ERR_INVALID);
    ASSERT_EQ(list.count, 0);
}

static void add_oversized_ssid_rejected(void)
{
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    char ssid[40];
    memset(ssid, 'A', sizeof(ssid) - 1);
    ssid[sizeof(ssid) - 1] = '\0';
    ll_wifi_add_result_t r = ll_wifi_list_add(&list, ssid, "p");
    ASSERT_EQ(r, LL_WIFI_ADD_ERR_INVALID);
    ASSERT_EQ(list.count, 0);
}

static void add_oversized_password_rejected(void)
{
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    char pw[80];
    memset(pw, 'B', sizeof(pw) - 1);
    pw[sizeof(pw) - 1] = '\0';
    ll_wifi_add_result_t r = ll_wifi_list_add(&list, "HomeWiFi", pw);
    ASSERT_EQ(r, LL_WIFI_ADD_ERR_INVALID);
    ASSERT_EQ(list.count, 0);
}

static void add_empty_password_allowed_for_open_network(void)
{
    /* Open networks have no PSK. Empty password must round-trip cleanly. */
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    ll_wifi_add_result_t r = ll_wifi_list_add(&list, "OpenCafe", "");
    ASSERT_EQ(r, LL_WIFI_ADD_OK_INSERTED);
    ASSERT_EQ(strlen(list.entries[0].password), 0);
}

/* ---- Remove -------------------------------------------------------------- */

static void remove_missing_returns_not_found(void)
{
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    seed(&list, "HomeWiFi", "p", 0);

    ll_wifi_remove_result_t r = ll_wifi_list_remove(&list, "GhostNet");
    ASSERT_EQ(r, LL_WIFI_REMOVE_NOT_FOUND);
    ASSERT_EQ(list.count, 1);
}

static void remove_compacts_in_order(void)
{
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    seed(&list, "n0", "p", 0);
    seed(&list, "n1", "p", 0);
    seed(&list, "n2", "p", 0);

    ll_wifi_remove_result_t r = ll_wifi_list_remove(&list, "n1");
    ASSERT_EQ(r, LL_WIFI_REMOVE_OK);
    ASSERT_EQ(list.count, 2);
    ASSERT_STR_EQ(list.entries[0].ssid, "n0");
    ASSERT_STR_EQ(list.entries[1].ssid, "n2");
    /* The vacated tail slot should be zeroed out — no stale ssid. */
    ASSERT_EQ(list.entries[2].ssid[0], '\0');
}

static void remove_active_clears_active_idx(void)
{
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    seed(&list, "n0", "p", 0);
    seed(&list, "n1", "p", 0);
    list.active_idx = 1;

    ll_wifi_list_remove(&list, "n1");
    ASSERT_EQ(list.active_idx, LL_WIFI_ACTIVE_NONE);
}

static void remove_before_active_decrements_active_idx(void)
{
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    seed(&list, "n0", "p", 0);
    seed(&list, "n1", "p", 0);
    seed(&list, "n2", "p", 0);
    list.active_idx = 2;  /* n2 */

    ll_wifi_list_remove(&list, "n0");
    /* n2 has shifted down to index 1; active_idx must follow. */
    ASSERT_EQ(list.active_idx, 1);
    ASSERT_STR_EQ(list.entries[1].ssid, "n2");
}

static void remove_after_active_leaves_active_idx_alone(void)
{
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    seed(&list, "n0", "p", 0);
    seed(&list, "n1", "p", 0);
    seed(&list, "n2", "p", 0);
    list.active_idx = 0;

    ll_wifi_list_remove(&list, "n2");
    ASSERT_EQ(list.active_idx, 0);
    ASSERT_STR_EQ(list.entries[0].ssid, "n0");
}

/* ---- Pick (priority) ----------------------------------------------------- */

static void pick_on_empty_returns_minus_one(void)
{
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    ASSERT_EQ(ll_wifi_list_pick_next(&list, 0), -1);
}

static void pick_returns_highest_last_used(void)
{
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    seed(&list, "old", "p", 100);
    seed(&list, "new", "p", 999);
    seed(&list, "med", "p", 500);
    ASSERT_EQ(ll_wifi_list_pick_next(&list, 0), 1);  /* "new" */
}

static void pick_skips_recently_failed(void)
{
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    seed(&list, "old", "p", 100);
    seed(&list, "new", "p", 999);   /* highest, but masked */
    seed(&list, "med", "p", 500);

    int idx = ll_wifi_list_pick_next(&list, (uint8_t)(1u << 1));
    ASSERT_EQ(idx, 2);  /* "med" wins after "new" is masked */
}

static void pick_returns_minus_one_when_all_failed(void)
{
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    seed(&list, "n0", "p", 100);
    seed(&list, "n1", "p", 200);
    int idx = ll_wifi_list_pick_next(&list, 0x03u);
    ASSERT_EQ(idx, -1);
}

static void pick_tiebreaks_to_lowest_index(void)
{
    /* Three never-connected entries: all last_used_us == 0. The lowest
     * index should win — deterministic and order-stable. */
    ll_wifi_list_t list;
    ll_wifi_list_init(&list);
    seed(&list, "n0", "p", 0);
    seed(&list, "n1", "p", 0);
    seed(&list, "n2", "p", 0);
    ASSERT_EQ(ll_wifi_list_pick_next(&list, 0), 0);
}

/* ---- Sanitize ------------------------------------------------------------ */

static void sanitize_truncates_non_null_terminated_ssid(void)
{
    ll_wifi_entry_t e;
    memset(&e, 'X', sizeof(e));     /* fill ssid + password with 'X', no terminator */

    ASSERT(ll_wifi_entry_sanitize(&e));
    /* The sanitizer only forces termination at the buffer boundary; it
     * does not scrub the rest of the buffer. So strlen(ssid) == 32. */
    ASSERT_EQ(strlen(e.ssid), 32);
    ASSERT_EQ(e.ssid[LL_WIFI_SSID_MAX], '\0');
    ASSERT_EQ(strlen(e.password), 64);
    ASSERT_EQ(e.password[LL_WIFI_PASSWORD_MAX], '\0');
}

static void sanitize_rejects_empty_ssid(void)
{
    ll_wifi_entry_t e;
    memset(&e, 0, sizeof(e));
    ASSERT(!ll_wifi_entry_sanitize(&e));
}

static void sanitize_handles_null(void)
{
    ASSERT(!ll_wifi_entry_sanitize(NULL));
}

void suite_ll_wifi_logic(void)
{
    RUN(entry_size_locked);
    RUN(init_starts_empty);

    RUN(find_on_empty_returns_minus_one);
    RUN(find_returns_index_when_present);
    RUN(find_is_case_sensitive);
    RUN(find_handles_null);

    RUN(add_to_empty_inserts);
    RUN(add_to_full_returns_full);
    RUN(add_existing_ssid_updates_password);
    RUN(add_empty_ssid_rejected);
    RUN(add_oversized_ssid_rejected);
    RUN(add_oversized_password_rejected);
    RUN(add_empty_password_allowed_for_open_network);

    RUN(remove_missing_returns_not_found);
    RUN(remove_compacts_in_order);
    RUN(remove_active_clears_active_idx);
    RUN(remove_before_active_decrements_active_idx);
    RUN(remove_after_active_leaves_active_idx_alone);

    RUN(pick_on_empty_returns_minus_one);
    RUN(pick_returns_highest_last_used);
    RUN(pick_skips_recently_failed);
    RUN(pick_returns_minus_one_when_all_failed);
    RUN(pick_tiebreaks_to_lowest_index);

    RUN(sanitize_truncates_non_null_terminated_ssid);
    RUN(sanitize_rejects_empty_ssid);
    RUN(sanitize_handles_null);
}
