#pragma once

/*
 * Paired-mode authentication — ESP-IDF side.
 *
 * Owns the `ll_auth` NVS namespace (key `shared_secret`) and the in-RAM
 * secret cache. The pure-C HMAC / canonicalization logic lives in
 * auth_logic.c so it is host-testable; this file is the device plumbing.
 *
 * Spec: docs/firmware-security.md §5 (Local Authentication).
 * Tracked by: LL-057-D (Sprint 8).
 *
 * Wiring order in main() — after ll_nvs_init()/ll_nvs_subscribe() (which
 * runs nvs_flash_init) and after ll_state_bus_init():
 *     ESP_ERROR_CHECK(ll_auth_init());
 *     ESP_ERROR_CHECK(ll_auth_subscribe());
 *
 * The transport dispatcher calls ll_auth_verify() on each incoming frame
 * when ll_auth_is_paired() is true.
 */

#include <stdbool.h>
#include <stddef.h>

#include "esp_err.h"

/* 63-char secret + NUL. Matches ll_ev_auth_mode_payload_t.secret[64]. */
#define LL_AUTH_SECRET_BUFSZ 64

typedef enum {
    LL_AUTH_OK        = 0,  /* signature present and valid                    */
    LL_AUTH_NO_SECRET = 1,  /* paired mode but no secret stored (degenerate)  */
    LL_AUTH_UNSIGNED  = 2,  /* frame carries no hmac field                    */
    LL_AUTH_BAD_HMAC  = 3,  /* hmac present but malformed or does not verify  */
} ll_auth_verdict_t;

/* Load the shared secret from NVS into the RAM cache. Idempotent. Always
 * returns ESP_OK for any non-fatal outcome (including "no secret stored");
 * a missing namespace just means the device is in open mode. */
esp_err_t ll_auth_init(void);

/* Subscribe to the state bus so a factory reset wipes the stored secret. */
esp_err_t ll_auth_subscribe(void);

/* True when the device's auth_mode is PAIRED. */
bool ll_auth_is_paired(void);

/* True when a non-empty shared secret is held in the cache. */
bool ll_auth_has_secret(void);

/* Persist a new shared secret to NVS and update the cache. Rejects a NULL,
 * empty, or over-long secret with ESP_ERR_INVALID_ARG. */
esp_err_t ll_auth_set_secret(const char *secret);

/* Erase the `ll_auth` namespace and zero the cache. Used by factory reset
 * and the paired-to-open transition. */
esp_err_t ll_auth_clear_secret(void);

/*
 * Verify the HMAC on a raw WebSocket frame against the cached secret.
 * `len` is the frame length in bytes.
 *
 * This covers the signature only. The `ts` replay window (firmware-security
 * §5.4) is layered on by the transport dispatcher in a later session, since
 * it needs per-connection context the auth module does not have.
 */
ll_auth_verdict_t ll_auth_verify(const char *raw_json, size_t len);
