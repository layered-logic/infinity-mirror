/*
 * Pure-C auth logic — see auth_logic.h.
 *
 * SHA-256 (FIPS 180-4) and HMAC-SHA256 (RFC 2104) implemented from scratch
 * so the crypto builds and is tested on the host. Verified against the
 * published SHA-256 and RFC 4231 HMAC test vectors in
 * tests/core/auth/test_auth_logic.c.
 */

#include "auth_logic.h"

#include <string.h>

/* ===== SHA-256 =========================================================== */

static const uint32_t SHA256_K[64] = {
    0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u,
    0x3956c25bu, 0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u,
    0xd807aa98u, 0x12835b01u, 0x243185beu, 0x550c7dc3u,
    0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u, 0xc19bf174u,
    0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu,
    0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau,
    0x983e5152u, 0xa831c66du, 0xb00327c8u, 0xbf597fc7u,
    0xc6e00bf3u, 0xd5a79147u, 0x06ca6351u, 0x14292967u,
    0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu, 0x53380d13u,
    0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
    0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u,
    0xd192e819u, 0xd6990624u, 0xf40e3585u, 0x106aa070u,
    0x19a4c116u, 0x1e376c08u, 0x2748774cu, 0x34b0bcb5u,
    0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu, 0x682e6ff3u,
    0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u,
    0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u,
};

#define ROTR32(x, n) (((x) >> (n)) | ((x) << (32 - (n))))

typedef struct {
    uint32_t h[8];
    uint64_t total_len;   /* total message bytes seen */
    uint8_t  buf[64];
    size_t   buf_len;
} sha256_ctx;

static void sha256_init(sha256_ctx *ctx)
{
    ctx->h[0] = 0x6a09e667u; ctx->h[1] = 0xbb67ae85u;
    ctx->h[2] = 0x3c6ef372u; ctx->h[3] = 0xa54ff53au;
    ctx->h[4] = 0x510e527fu; ctx->h[5] = 0x9b05688cu;
    ctx->h[6] = 0x1f83d9abu; ctx->h[7] = 0x5be0cd19u;
    ctx->total_len = 0;
    ctx->buf_len = 0;
}

static void sha256_block(sha256_ctx *ctx, const uint8_t *p)
{
    uint32_t w[64];
    for (int i = 0; i < 16; i++) {
        w[i] = ((uint32_t)p[i * 4] << 24)
             | ((uint32_t)p[i * 4 + 1] << 16)
             | ((uint32_t)p[i * 4 + 2] << 8)
             | ((uint32_t)p[i * 4 + 3]);
    }
    for (int i = 16; i < 64; i++) {
        uint32_t s0 = ROTR32(w[i - 15], 7) ^ ROTR32(w[i - 15], 18) ^ (w[i - 15] >> 3);
        uint32_t s1 = ROTR32(w[i - 2], 17) ^ ROTR32(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    uint32_t a = ctx->h[0], b = ctx->h[1], c = ctx->h[2], d = ctx->h[3];
    uint32_t e = ctx->h[4], f = ctx->h[5], g = ctx->h[6], h = ctx->h[7];

    for (int i = 0; i < 64; i++) {
        uint32_t s1  = ROTR32(e, 6) ^ ROTR32(e, 11) ^ ROTR32(e, 25);
        uint32_t ch  = (e & f) ^ (~e & g);
        uint32_t t1  = h + s1 + ch + SHA256_K[i] + w[i];
        uint32_t s0  = ROTR32(a, 2) ^ ROTR32(a, 13) ^ ROTR32(a, 22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t t2  = s0 + maj;
        h = g; g = f; f = e; e = d + t1;
        d = c; c = b; b = a; a = t1 + t2;
    }

    ctx->h[0] += a; ctx->h[1] += b; ctx->h[2] += c; ctx->h[3] += d;
    ctx->h[4] += e; ctx->h[5] += f; ctx->h[6] += g; ctx->h[7] += h;
}

static void sha256_update(sha256_ctx *ctx, const uint8_t *data, size_t len)
{
    ctx->total_len += len;
    while (len > 0) {
        size_t take = 64 - ctx->buf_len;
        if (take > len) take = len;
        memcpy(ctx->buf + ctx->buf_len, data, take);
        ctx->buf_len += take;
        data += take;
        len  -= take;
        if (ctx->buf_len == 64) {
            sha256_block(ctx, ctx->buf);
            ctx->buf_len = 0;
        }
    }
}

static void sha256_final(sha256_ctx *ctx, uint8_t out[32])
{
    uint64_t bit_len = ctx->total_len * 8u;

    /* 0x80 terminator, then zero-pad until 56 bytes into the block. */
    uint8_t pad = 0x80;
    sha256_update(ctx, &pad, 1);
    uint8_t zero = 0x00;
    while (ctx->buf_len != 56) {
        sha256_update(ctx, &zero, 1);
    }

    /* 64-bit big-endian message length in bits. */
    uint8_t len_be[8];
    for (int i = 0; i < 8; i++) {
        len_be[i] = (uint8_t)(bit_len >> (56 - 8 * i));
    }
    sha256_update(ctx, len_be, 8);

    for (int i = 0; i < 8; i++) {
        out[i * 4]     = (uint8_t)(ctx->h[i] >> 24);
        out[i * 4 + 1] = (uint8_t)(ctx->h[i] >> 16);
        out[i * 4 + 2] = (uint8_t)(ctx->h[i] >> 8);
        out[i * 4 + 3] = (uint8_t)(ctx->h[i]);
    }
}

void ll_sha256(const uint8_t *data, size_t len, uint8_t out[32])
{
    sha256_ctx ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, data, len);
    sha256_final(&ctx, out);
}

/* ===== HMAC-SHA256 ======================================================= */

void ll_hmac_sha256(const uint8_t *key, size_t key_len,
                    const uint8_t *msg, size_t msg_len, uint8_t out[32])
{
    uint8_t k[64];
    memset(k, 0, sizeof(k));
    if (key_len > 64) {
        ll_sha256(key, key_len, k);          /* k = H(key), tail stays zero */
    } else if (key_len > 0) {
        memcpy(k, key, key_len);
    }

    uint8_t ipad[64], opad[64];
    for (int i = 0; i < 64; i++) {
        ipad[i] = (uint8_t)(k[i] ^ 0x36);
        opad[i] = (uint8_t)(k[i] ^ 0x5c);
    }

    uint8_t inner[32];
    sha256_ctx ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, ipad, 64);
    sha256_update(&ctx, msg, msg_len);
    sha256_final(&ctx, inner);

    sha256_init(&ctx);
    sha256_update(&ctx, opad, 64);
    sha256_update(&ctx, inner, 32);
    sha256_final(&ctx, out);
}

/* ===== Hex =============================================================== */

void ll_hex_encode(const uint8_t *in, size_t n, char *out)
{
    static const char digits[] = "0123456789abcdef";
    for (size_t i = 0; i < n; i++) {
        out[i * 2]     = digits[(in[i] >> 4) & 0x0F];
        out[i * 2 + 1] = digits[in[i] & 0x0F];
    }
    out[n * 2] = '\0';
}

static int hex_nibble(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

bool ll_hex_decode(const char *hex, size_t hex_len, uint8_t *out, size_t out_n)
{
    if ((hex_len & 1u) != 0) return false;
    if (hex_len / 2 != out_n) return false;
    for (size_t i = 0; i < out_n; i++) {
        int hi = hex_nibble(hex[i * 2]);
        int lo = hex_nibble(hex[i * 2 + 1]);
        if (hi < 0 || lo < 0) return false;
        out[i] = (uint8_t)((hi << 4) | lo);
    }
    return true;
}

/* ===== Constant-time compare ============================================ */

bool ll_consttime_eq(const uint8_t *a, const uint8_t *b, size_t n)
{
    uint8_t diff = 0;
    for (size_t i = 0; i < n; i++) {
        diff |= (uint8_t)(a[i] ^ b[i]);
    }
    return diff == 0;
}

/* ===== Envelope canonicalization ======================================== */

bool ll_auth_split_envelope(const char *json, size_t len,
                            size_t *region_len,
                            const char **hmac_hex, size_t *hmac_hex_len)
{
    static const char TOKEN[] = ",\"hmac\":\"";
    const size_t tok = sizeof(TOKEN) - 1;   /* 9 */

    /* Minimum signed frame: token + at least one hex char + closing `"}`. */
    if (len < tok + 3) return false;
    if (json[len - 1] != '}' || json[len - 2] != '"') return false;

    /* Scan backward from the latest token start that still leaves room for
     * one hex char before the closing `"}`. The real `,"hmac":` is the
     * rightmost one (hmac is the final key by contract); a literal copy
     * inside an earlier payload string sits to the left and loses. */
    size_t i = len - 3 - tok;
    for (;;) {
        if (memcmp(json + i, TOKEN, tok) == 0) {
            size_t hex_start = i + tok;
            *region_len   = i;
            *hmac_hex     = json + hex_start;
            *hmac_hex_len = (len - 2) - hex_start;
            return true;
        }
        if (i == 0) break;
        i--;
    }
    return false;
}

ll_sig_result_t ll_auth_check_signature(const char *secret, size_t secret_len,
                                        const char *json, size_t len)
{
    size_t region_len = 0, hmac_hex_len = 0;
    const char *hmac_hex = NULL;

    if (!ll_auth_split_envelope(json, len, &region_len, &hmac_hex, &hmac_hex_len)) {
        return LL_SIG_MISSING;
    }
    if (hmac_hex_len != 64) {
        return LL_SIG_MALFORMED;   /* HMAC-SHA256 is always 32 bytes */
    }

    uint8_t provided[32];
    if (!ll_hex_decode(hmac_hex, 64, provided, 32)) {
        return LL_SIG_MALFORMED;
    }

    uint8_t computed[32];
    ll_hmac_sha256((const uint8_t *)secret, secret_len,
                   (const uint8_t *)json, region_len, computed);

    return ll_consttime_eq(provided, computed, 32) ? LL_SIG_OK : LL_SIG_MISMATCH;
}
