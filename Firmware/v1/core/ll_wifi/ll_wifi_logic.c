#include "ll_wifi_logic.h"

#include <string.h>

void ll_wifi_list_init(ll_wifi_list_t *list)
{
    if (list == NULL) {
        return;
    }
    memset(list, 0, sizeof(*list));
    list->active_idx = LL_WIFI_ACTIVE_NONE;
}

int ll_wifi_list_find(const ll_wifi_list_t *list, const char *ssid)
{
    if (list == NULL || ssid == NULL) {
        return -1;
    }
    for (uint8_t i = 0; i < list->count && i < LL_WIFI_LIST_MAX; i++) {
        /* strncmp with the field width: handles a non-null-terminated
         * stored ssid identically to a well-formed one. */
        if (strncmp(list->entries[i].ssid, ssid, LL_WIFI_SSID_MAX) == 0) {
            return (int)i;
        }
    }
    return -1;
}

ll_wifi_add_result_t ll_wifi_list_add(ll_wifi_list_t *list,
                                       const char *ssid,
                                       const char *password)
{
    if (list == NULL || ssid == NULL || password == NULL) {
        return LL_WIFI_ADD_ERR_INVALID;
    }
    /* +1 so we can detect strings that exceed the cap. */
    size_t ssid_len = strnlen(ssid, LL_WIFI_SSID_MAX + 1);
    size_t pw_len   = strnlen(password, LL_WIFI_PASSWORD_MAX + 1);
    if (ssid_len == 0 || ssid_len > LL_WIFI_SSID_MAX
        || pw_len > LL_WIFI_PASSWORD_MAX) {
        return LL_WIFI_ADD_ERR_INVALID;
    }

    int idx = ll_wifi_list_find(list, ssid);
    if (idx >= 0) {
        ll_wifi_entry_t *e = &list->entries[idx];
        memset(e->password, 0, sizeof(e->password));
        memcpy(e->password, password, pw_len);
        return LL_WIFI_ADD_OK_UPDATED;
    }

    if (list->count >= LL_WIFI_LIST_MAX) {
        return LL_WIFI_ADD_ERR_FULL;
    }

    ll_wifi_entry_t *e = &list->entries[list->count];
    memset(e, 0, sizeof(*e));
    memcpy(e->ssid, ssid, ssid_len);
    memcpy(e->password, password, pw_len);
    e->last_used_us = 0;
    list->count++;
    return LL_WIFI_ADD_OK_INSERTED;
}

ll_wifi_remove_result_t ll_wifi_list_remove(ll_wifi_list_t *list,
                                             const char *ssid)
{
    if (list == NULL) {
        return LL_WIFI_REMOVE_NOT_FOUND;
    }
    int idx = ll_wifi_list_find(list, ssid);
    if (idx < 0) {
        return LL_WIFI_REMOVE_NOT_FOUND;
    }

    /* Shift entries down past the removed slot. */
    for (uint8_t i = (uint8_t)idx; i + 1 < list->count; i++) {
        list->entries[i] = list->entries[i + 1];
    }
    memset(&list->entries[list->count - 1], 0, sizeof(list->entries[0]));
    list->count--;

    /* Track active_idx through the shift. */
    if (list->active_idx == LL_WIFI_ACTIVE_NONE) {
        /* nothing to fix */
    } else if (list->active_idx == (uint8_t)idx) {
        list->active_idx = LL_WIFI_ACTIVE_NONE;
    } else if (list->active_idx > (uint8_t)idx) {
        list->active_idx--;
    }
    return LL_WIFI_REMOVE_OK;
}

int ll_wifi_list_pick_next(const ll_wifi_list_t *list,
                            uint8_t recently_failed_mask)
{
    if (list == NULL || list->count == 0) {
        return -1;
    }
    int best = -1;
    int64_t best_last = 0;
    for (uint8_t i = 0; i < list->count && i < LL_WIFI_LIST_MAX; i++) {
        if (recently_failed_mask & (1u << i)) {
            continue;
        }
        /* Strict > so on a tie the lowest index wins (see header note). */
        if (best < 0 || list->entries[i].last_used_us > best_last) {
            best = (int)i;
            best_last = list->entries[i].last_used_us;
        }
    }
    return best;
}

bool ll_wifi_entry_sanitize(ll_wifi_entry_t *entry)
{
    if (entry == NULL) {
        return false;
    }
    entry->ssid[LL_WIFI_SSID_MAX] = '\0';
    entry->password[LL_WIFI_PASSWORD_MAX] = '\0';
    return entry->ssid[0] != '\0';
}
