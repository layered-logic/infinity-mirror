#pragma once

/*
 * Pure-C auth logic: SHA-256, HMAC-SHA256, hex, constant-time compare, and
 * the paired-mode envelope canonicalization.
 *
 * Split from auth.c so it builds on the host for unit tests (see
 * tests/core/auth/test_auth_logic.c). auth.c owns the NVS secret store, the
 * RAM cache, and the state-bus wiring; everything that is pure computation —
 * and therefore the highest-risk correctness surface — lives here.
 *
 * Spec: docs/firmware-security.md §5 (Local Authentication).
 * Tracked by: LL-057-D (Sprint 8).
 *
 * SHA-256 is FIPS 180-4; HMAC-SHA256 is RFC 2104 / RFC 4231. Implemented
 * here as pure C (no mbedtls) specifically so the crypto is host-testable
 * against published test vectors and so it can serve as the byte-for-byte
 * reference the app's JS HMAC must match.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ---- SHA-256 / HMAC-SHA256 ---------------------------------------------- */

/* SHA-256 digest of `data` (FIPS 180-4). `out` receives 32 bytes. */
void ll_sha256(const uint8_t *data, size_t len, uint8_t out[32]);

/* HMAC-SHA256 of `msg` keyed on `key` (RFC 2104). `out` receives 32 bytes. */
void ll_hmac_sha256(const uint8_t *key, size_t key_len,
                    const uint8_t *msg, size_t msg_len, uint8_t out[32]);

/* ---- Hex ---------------------------------------------------------------- */

/* Lowercase-hex-encode `n` bytes into `out`. `out` must hold 2*n + 1 bytes
 * (the trailing NUL is written). */
void ll_hex_encode(const uint8_t *in, size_t n, char *out);

/* Decode `hex_len` hex chars into `out` (`out_n` bytes). Returns false if
 * hex_len is odd, hex_len/2 != out_n, or any character is not a hex digit.
 * Accepts upper- or lower-case input. */
bool ll_hex_decode(const char *hex, size_t hex_len, uint8_t *out, size_t out_n);

/* ---- Constant-time compare --------------------------------------------- */

/* Compare `n` bytes without an early exit on the first mismatch — the timing
 * does not reveal how many leading bytes matched. Returns true if equal. */
bool ll_consttime_eq(const uint8_t *a, const uint8_t *b, size_t n);

/* ---- Paired-mode envelope canonicalization ------------------------------ */

/*
 * The HMAC covers "the message minus the hmac field" (firmware-security
 * §5.4). JSON has no canonical byte form, so device and app agree on one
 * structural rule instead: the `hmac` field is ALWAYS the last key, and the
 * signed bytes are exactly the envelope text before the `,"hmac":` token.
 *
 * Concretely — the app builds the envelope object {op, req_id, ts, payload},
 * serializes it, drops the trailing `}`, and HMACs that brace-less string;
 * then it transmits  <signed-string> + `,"hmac":"` + <hex> + `"}`.  The
 * device signs the exact received prefix, so the two byte strings match
 * with no re-serialization on either side. The signed region is therefore
 * NOT itself valid JSON (it has no closing brace) — that is intentional;
 * HMAC signs bytes, not JSON.
 *
 * ll_auth_split_envelope locates that split in a received frame. On success:
 *   - *region_len  = number of leading bytes that were HMAC'd
 *   - *hmac_hex    = pointer into `json` at the start of the hex digest
 *   - *hmac_hex_len = length of that hex run
 *
 * Returns false if the frame has no trailing `,"hmac":"<hex>"}` shape — i.e.
 * the envelope is unsigned or malformed. Searches from the end so a literal
 * `,"hmac":` sitting inside a payload string does not get mistaken for the
 * real field (it would also have to be followed by valid hex + `"}` at the
 * exact buffer end).
 */
bool ll_auth_split_envelope(const char *json, size_t len,
                            size_t *region_len,
                            const char **hmac_hex, size_t *hmac_hex_len);

typedef enum {
    LL_SIG_OK        = 0,  /* hmac present, well-formed, and correct          */
    LL_SIG_MISSING   = 1,  /* no `,"hmac":` token — envelope is unsigned      */
    LL_SIG_MALFORMED = 2,  /* token present but trailing shape / hex is wrong */
    LL_SIG_MISMATCH  = 3,  /* well-formed hmac, but it does not verify        */
} ll_sig_result_t;

/*
 * Full signature check: split the envelope, HMAC the signed region with
 * `secret`, and constant-time compare against the supplied digest.
 * `len` is the length of the raw JSON frame. A 32-byte (64 hex char)
 * digest is required; any other length is LL_SIG_MALFORMED.
 */
ll_sig_result_t ll_auth_check_signature(const char *secret, size_t secret_len,
                                        const char *json, size_t len);
