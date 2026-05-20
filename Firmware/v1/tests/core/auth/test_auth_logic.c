#include <stdio.h>
#include <string.h>

#include "auth_logic.h"
#include "test_harness.h"

/*
 * Host tests for core/auth/auth_logic.c — SHA-256, HMAC-SHA256, hex,
 * constant-time compare, and the paired-mode envelope canonicalization.
 *
 * The SHA-256 and HMAC primitives are checked against published
 * known-answer vectors (FIPS 180-4 examples + RFC 4231). The envelope
 * path is then round-trip verified: because the primitives are externally
 * anchored, a sign-then-check round trip is a sound test of the split +
 * verify logic without needing a second HMAC oracle.
 *
 * Tracked by: LL-057-D session D1 (Sprint 8).
 */

/* ---- Helpers ---- */

static void sha_hex(const char *s, char out[65])
{
    uint8_t d[32];
    ll_sha256((const uint8_t *)s, strlen(s), d);
    ll_hex_encode(d, 32, out);
}

static void hmac_hex(const uint8_t *key, size_t kl,
                     const uint8_t *msg, size_t ml, char out[65])
{
    uint8_t d[32];
    ll_hmac_sha256(key, kl, msg, ml, d);
    ll_hex_encode(d, 32, out);
}

/* Build a signed envelope from a brace-less signed region — the region is
 * the JSON object text with its trailing '}' removed, exactly what the app
 * HMACs (see auth_logic.h). */
static void build_signed(const char *secret, const char *region,
                         char *out, size_t out_sz)
{
    uint8_t mac[32];
    char machex[65];
    ll_hmac_sha256((const uint8_t *)secret, strlen(secret),
                   (const uint8_t *)region, strlen(region), mac);
    ll_hex_encode(mac, 32, machex);
    snprintf(out, out_sz, "%s,\"hmac\":\"%s\"}", region, machex);
}

/* ---- SHA-256 known-answer vectors ---- */

static void sha256_empty_string(void)
{
    char hex[65];
    sha_hex("", hex);
    ASSERT_STR_EQ(hex,
        "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

static void sha256_abc(void)
{
    char hex[65];
    sha_hex("abc", hex);
    ASSERT_STR_EQ(hex,
        "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

static void sha256_multiblock(void)
{
    /* 56-byte input: after the 0x80 terminator the length field spills
     * into a second block — exercises the padding edge. */
    char hex[65];
    sha_hex("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", hex);
    ASSERT_STR_EQ(hex,
        "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
}

/* ---- HMAC-SHA256 known-answer vectors (RFC 4231) ---- */

static void hmac_rfc4231_case1(void)
{
    uint8_t key[20];
    memset(key, 0x0b, sizeof(key));
    char hex[65];
    hmac_hex(key, sizeof(key), (const uint8_t *)"Hi There", 8, hex);
    ASSERT_STR_EQ(hex,
        "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7");
}

static void hmac_rfc4231_case2(void)
{
    const char *msg = "what do ya want for nothing?";
    char hex[65];
    hmac_hex((const uint8_t *)"Jefe", 4, (const uint8_t *)msg, strlen(msg), hex);
    ASSERT_STR_EQ(hex,
        "5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843");
}

static void hmac_rfc4231_case6_long_key(void)
{
    /* 131-byte key forces the "key longer than the 64-byte block" path
     * where the key is itself hashed first. */
    uint8_t key[131];
    memset(key, 0xaa, sizeof(key));
    const char *msg =
        "Test Using Larger Than Block-Size Key - Hash Key First";
    char hex[65];
    hmac_hex(key, sizeof(key), (const uint8_t *)msg, strlen(msg), hex);
    ASSERT_STR_EQ(hex,
        "60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54");
}

/* ---- Hex ---- */

static void hex_encode_roundtrip(void)
{
    uint8_t in[4] = {0x00, 0x7f, 0xab, 0xff};
    char hex[9];
    ll_hex_encode(in, 4, hex);
    ASSERT_STR_EQ(hex, "007fabff");

    uint8_t out[4];
    ASSERT(ll_hex_decode(hex, 8, out, 4));
    ASSERT(out[0] == 0x00 && out[1] == 0x7f && out[2] == 0xab && out[3] == 0xff);
}

static void hex_decode_accepts_uppercase(void)
{
    uint8_t out[2];
    ASSERT(ll_hex_decode("AbCd", 4, out, 2));
    ASSERT_EQ(out[0], 0xAB);
    ASSERT_EQ(out[1], 0xCD);
}

static void hex_decode_rejects_odd_length(void)
{
    uint8_t out[2];
    ASSERT(!ll_hex_decode("abc", 3, out, 2));
}

static void hex_decode_rejects_bad_chars(void)
{
    uint8_t out[2];
    ASSERT(!ll_hex_decode("0g1f", 4, out, 2));
}

static void hex_decode_rejects_size_mismatch(void)
{
    uint8_t out[2];
    ASSERT(!ll_hex_decode("abcdef", 6, out, 2));   /* 3 bytes of hex, out_n = 2 */
}

/* ---- Constant-time compare ---- */

static void consttime_eq_matches_and_differs(void)
{
    uint8_t a[4] = {1, 2, 3, 4};
    uint8_t b[4] = {1, 2, 3, 4};
    uint8_t c[4] = {1, 2, 3, 9};   /* differs only in the last byte */
    uint8_t d[4] = {9, 2, 3, 4};   /* differs only in the first byte */
    ASSERT(ll_consttime_eq(a, b, 4));
    ASSERT(!ll_consttime_eq(a, c, 4));
    ASSERT(!ll_consttime_eq(a, d, 4));
}

/* ---- Envelope split ---- */

static void split_envelope_valid(void)
{
    const char *region = "{\"op\":\"ping\",\"req_id\":\"r1\",\"ts\":100";
    char env[256];
    build_signed("swordfish", region, env, sizeof(env));

    size_t region_len = 0, hex_len = 0;
    const char *hex = NULL;
    ASSERT(ll_auth_split_envelope(env, strlen(env), &region_len, &hex, &hex_len));
    ASSERT_EQ(region_len, strlen(region));
    ASSERT_EQ(hex_len, 64);
    ASSERT(memcmp(env, region, region_len) == 0);
}

static void split_envelope_missing_hmac(void)
{
    const char *env = "{\"op\":\"ping\",\"ts\":100}";
    size_t region_len = 0, hex_len = 0;
    const char *hex = NULL;
    ASSERT(!ll_auth_split_envelope(env, strlen(env), &region_len, &hex, &hex_len));
}

static void split_envelope_rejects_hmac_not_last(void)
{
    /* hmac field present but a field follows it → not a valid signed frame */
    const char *env = "{\"op\":\"ping\",\"hmac\":\"abcd\",\"ts\":100}";
    size_t region_len = 0, hex_len = 0;
    const char *hex = NULL;
    ASSERT(!ll_auth_split_envelope(env, strlen(env), &region_len, &hex, &hex_len));
}

/* ---- Full signature check ---- */

static void check_signature_valid(void)
{
    const char *region = "{\"op\":\"set_state\",\"req_id\":\"r2\",\"ts\":500";
    char env[256];
    build_signed("hunter2", region, env, sizeof(env));
    ASSERT_EQ(ll_auth_check_signature("hunter2", 7, env, strlen(env)), LL_SIG_OK);
}

static void check_signature_wrong_secret(void)
{
    const char *region = "{\"op\":\"set_state\",\"req_id\":\"r2\",\"ts\":500";
    char env[256];
    build_signed("hunter2", region, env, sizeof(env));
    ASSERT_EQ(ll_auth_check_signature("wrongkey", 8, env, strlen(env)),
              LL_SIG_MISMATCH);
}

static void check_signature_tampered_region(void)
{
    const char *region = "{\"op\":\"set_state\",\"req_id\":\"r2\",\"ts\":500";
    char env[256];
    build_signed("hunter2", region, env, sizeof(env));
    /* Flip a byte inside the signed region — the 'ts' value. */
    char *ts = strstr(env, "500");
    ASSERT(ts != NULL);
    ts[0] = '9';
    ASSERT_EQ(ll_auth_check_signature("hunter2", 7, env, strlen(env)),
              LL_SIG_MISMATCH);
}

static void check_signature_missing(void)
{
    const char *env = "{\"op\":\"ping\",\"ts\":100}";
    ASSERT_EQ(ll_auth_check_signature("hunter2", 7, env, strlen(env)),
              LL_SIG_MISSING);
}

static void check_signature_short_digest(void)
{
    /* hmac present and last, but only 8 hex chars instead of 64. */
    const char *env = "{\"op\":\"ping\",\"ts\":100,\"hmac\":\"abcdef01\"}";
    ASSERT_EQ(ll_auth_check_signature("hunter2", 7, env, strlen(env)),
              LL_SIG_MALFORMED);
}

static void check_signature_non_hex_digest(void)
{
    /* 64 chars in the hmac slot, but not hex digits. */
    char zs[65];
    memset(zs, 'z', 64);
    zs[64] = '\0';
    char env[160];
    snprintf(env, sizeof(env), "{\"op\":\"ping\",\"ts\":100,\"hmac\":\"%s\"}", zs);
    ASSERT_EQ(ll_auth_check_signature("hunter2", 7, env, strlen(env)),
              LL_SIG_MALFORMED);
}

void suite_auth_logic(void)
{
    RUN(sha256_empty_string);
    RUN(sha256_abc);
    RUN(sha256_multiblock);

    RUN(hmac_rfc4231_case1);
    RUN(hmac_rfc4231_case2);
    RUN(hmac_rfc4231_case6_long_key);

    RUN(hex_encode_roundtrip);
    RUN(hex_decode_accepts_uppercase);
    RUN(hex_decode_rejects_odd_length);
    RUN(hex_decode_rejects_bad_chars);
    RUN(hex_decode_rejects_size_mismatch);

    RUN(consttime_eq_matches_and_differs);

    RUN(split_envelope_valid);
    RUN(split_envelope_missing_hmac);
    RUN(split_envelope_rejects_hmac_not_last);

    RUN(check_signature_valid);
    RUN(check_signature_wrong_secret);
    RUN(check_signature_tampered_region);
    RUN(check_signature_missing);
    RUN(check_signature_short_digest);
    RUN(check_signature_non_hex_digest);
}
