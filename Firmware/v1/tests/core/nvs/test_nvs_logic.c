#include <string.h>

#include "nvs_logic.h"
#include "state_bus.h"
#include "test_harness.h"

/*
 * NVS logic tests — schema versioning, round-trip serialization, and
 * deserialize sanitization of weird on-flash values.
 *
 * These tests exercise nvs_logic.c on the host, with zero ESP-IDF deps.
 * The runtime flash handling in nvs.c (nvs_flash_init, debounce timer,
 * subscriber) is target-only and not covered here.
 */

/* ---- Helpers ---- */

static void make_sane_state(ll_state_t *s)
{
    memset(s, 0, sizeof(*s));
    s->on = true;
    strncpy(s->pattern_id, "rainbow", sizeof(s->pattern_id) - 1);
    s->base_color_rgb = 0xFF00AA;
    s->brightness = 50;
    s->led_count = 66;           /* 12x12 test prototype */
    s->auth_mode = LL_AUTH_PAIRED;
    s->telemetry_enabled = true;
    strncpy(s->name, "Living Room", sizeof(s->name) - 1);
}

static bool states_equal(const ll_state_t *a, const ll_state_t *b)
{
    return a->on == b->on
        && a->base_color_rgb == b->base_color_rgb
        && a->brightness == b->brightness
        && a->led_count == b->led_count
        && a->auth_mode == b->auth_mode
        && a->telemetry_enabled == b->telemetry_enabled
        && strncmp(a->pattern_id, b->pattern_id, sizeof(a->pattern_id)) == 0
        && strncmp(a->name, b->name, sizeof(a->name)) == 0;
}

/* ---- Serialization ---- */

static void serialize_populates_schema_version(void)
{
    ll_state_t s; make_sane_state(&s);
    ll_nvs_blob_t blob;
    ll_nvs_serialize(&s, &blob);
    ASSERT_EQ(blob.schema_version, LL_NVS_SCHEMA_VERSION);
}

static void serialize_copies_all_fields(void)
{
    ll_state_t s; make_sane_state(&s);
    ll_nvs_blob_t blob;
    ll_nvs_serialize(&s, &blob);
    ASSERT_EQ(blob.on, 1);
    ASSERT_EQ(blob.brightness, 50);
    ASSERT_EQ_HEX(blob.base_color_rgb, 0xFF00AA);
    ASSERT_EQ(blob.led_count, 66);
    ASSERT_EQ(blob.auth_mode, (uint8_t)LL_AUTH_PAIRED);
    ASSERT_EQ(blob.telemetry_enabled, 1);
    ASSERT_STR_EQ(blob.pattern_id, "rainbow");
    ASSERT_STR_EQ(blob.name, "Living Room");
}

static void roundtrip_preserves_all_fields(void)
{
    ll_state_t original; make_sane_state(&original);
    ll_nvs_blob_t blob;
    ll_nvs_serialize(&original, &blob);

    ll_state_t loaded;
    ASSERT(ll_nvs_deserialize(&blob, sizeof(blob), &loaded));
    ASSERT(states_equal(&original, &loaded));
}

static void blob_size_matches_compile_time_constant(void)
{
    /* Lock the wire size so accidental layout changes fail loudly in
     * review instead of at a user's bricked infinity mirror.
     * Schema v2: 44 (v1) + 33 (name field) = 77. */
    ASSERT_EQ(sizeof(ll_nvs_blob_t), 77);
}

/* ---- Hard failures ---- */

static void null_data_returns_false(void)
{
    ll_state_t out = {0};
    ASSERT(!ll_nvs_deserialize(NULL, sizeof(ll_nvs_blob_t), &out));
}

static void wrong_size_returns_false(void)
{
    ll_state_t s; make_sane_state(&s);
    ll_nvs_blob_t blob;
    ll_nvs_serialize(&s, &blob);

    ll_state_t out = {0};
    ASSERT(!ll_nvs_deserialize(&blob, sizeof(blob) - 1, &out));
    ASSERT(!ll_nvs_deserialize(&blob, sizeof(blob) + 4, &out));
}

static void schema_mismatch_returns_false(void)
{
    ll_state_t s; make_sane_state(&s);
    ll_nvs_blob_t blob;
    ll_nvs_serialize(&s, &blob);
    blob.schema_version = LL_NVS_SCHEMA_VERSION + 1;

    ll_state_t out = {0};
    ASSERT(!ll_nvs_deserialize(&blob, sizeof(blob), &out));
}

static void schema_zero_returns_false(void)
{
    /* All-zero blob (e.g., stray flash erase) must fail, not return a
     * silently half-valid state. */
    ll_nvs_blob_t blob;
    memset(&blob, 0, sizeof(blob));

    ll_state_t out = {0};
    ASSERT(!ll_nvs_deserialize(&blob, sizeof(blob), &out));
}

/* ---- Soft sanitization ---- */

static void brightness_clamped_to_100(void)
{
    ll_state_t s; make_sane_state(&s);
    ll_nvs_blob_t blob;
    ll_nvs_serialize(&s, &blob);
    blob.brightness = 250;

    ll_state_t out = {0};
    ASSERT(ll_nvs_deserialize(&blob, sizeof(blob), &out));
    ASSERT_EQ(out.brightness, 100);
}

static void zero_led_count_becomes_default(void)
{
    ll_state_t s; make_sane_state(&s);
    ll_nvs_blob_t blob;
    ll_nvs_serialize(&s, &blob);
    blob.led_count = 0;

    ll_state_t out = {0};
    ASSERT(ll_nvs_deserialize(&blob, sizeof(blob), &out));
    ASSERT_EQ(out.led_count, 32);
}

static void oversized_led_count_becomes_default(void)
{
    ll_state_t s; make_sane_state(&s);
    ll_nvs_blob_t blob;
    ll_nvs_serialize(&s, &blob);
    blob.led_count = 50000;  /* will wrap in uint16_t, still far out of range */

    ll_state_t out = {0};
    ASSERT(ll_nvs_deserialize(&blob, sizeof(blob), &out));
    ASSERT_EQ(out.led_count, 32);
}

static void invalid_auth_mode_becomes_open(void)
{
    ll_state_t s; make_sane_state(&s);  /* PAIRED */
    ll_nvs_blob_t blob;
    ll_nvs_serialize(&s, &blob);
    blob.auth_mode = 42;

    ll_state_t out = {0};
    ASSERT(ll_nvs_deserialize(&blob, sizeof(blob), &out));
    ASSERT_EQ(out.auth_mode, LL_AUTH_OPEN);
}

static void empty_pattern_id_becomes_solid(void)
{
    ll_state_t s; make_sane_state(&s);
    ll_nvs_blob_t blob;
    ll_nvs_serialize(&s, &blob);
    blob.pattern_id[0] = '\0';

    ll_state_t out = {0};
    ASSERT(ll_nvs_deserialize(&blob, sizeof(blob), &out));
    ASSERT_STR_EQ(out.pattern_id, "solid");
}

static void non_null_terminated_pattern_id_is_truncated(void)
{
    ll_state_t s; make_sane_state(&s);
    ll_nvs_blob_t blob;
    ll_nvs_serialize(&s, &blob);
    memset(blob.pattern_id, 'A', sizeof(blob.pattern_id));  /* 32 chars, no terminator */

    ll_state_t out = {0};
    ASSERT(ll_nvs_deserialize(&blob, sizeof(blob), &out));
    /* last byte must be the terminator we force, 31 As before it */
    ASSERT_EQ(strlen(out.pattern_id), 31);
    ASSERT_EQ(out.pattern_id[31], '\0');
}

static void upper_byte_of_color_ignored(void)
{
    ll_state_t s; make_sane_state(&s);
    ll_nvs_blob_t blob;
    ll_nvs_serialize(&s, &blob);
    blob.base_color_rgb = 0xDEADBEEF;  /* upper byte should get masked */

    ll_state_t out = {0};
    ASSERT(ll_nvs_deserialize(&blob, sizeof(blob), &out));
    ASSERT_EQ_HEX(out.base_color_rgb, 0xADBEEF);
}

static void empty_name_survives_roundtrip(void)
{
    /* Empty name is a legal value (signals "fall back to id" client-side);
     * deserialize must not substitute anything. */
    ll_state_t s; make_sane_state(&s);
    s.name[0] = '\0';
    ll_nvs_blob_t blob;
    ll_nvs_serialize(&s, &blob);

    ll_state_t out = {0};
    ASSERT(ll_nvs_deserialize(&blob, sizeof(blob), &out));
    ASSERT_EQ(strlen(out.name), 0);
}

static void non_null_terminated_name_is_truncated(void)
{
    ll_state_t s; make_sane_state(&s);
    ll_nvs_blob_t blob;
    ll_nvs_serialize(&s, &blob);
    /* Splat 33 bytes of A across the name slot, no terminator anywhere. */
    memset(blob.name, 'A', sizeof(blob.name));

    ll_state_t out = {0};
    ASSERT(ll_nvs_deserialize(&blob, sizeof(blob), &out));
    ASSERT_EQ(strlen(out.name), 32);
    ASSERT_EQ(out.name[32], '\0');
}

/* ---- Defaults ---- */

static void defaults_are_shipping_values(void)
{
    /* Defaults must be shipping-ready: power-off at first boot (won't
     * surprise an unboxing user), pattern=solid, brightness=75, 32 LEDs
     * (6x6 product SKU). Brand color checked separately since it's a
     * moving target until the hardware ship date. */
    ll_state_t s = {0};
    ll_state_defaults(&s);
    ASSERT(!s.on);
    ASSERT_STR_EQ(s.pattern_id, "solid");
    ASSERT_EQ(s.brightness, 75);
    ASSERT_EQ(s.led_count, 32);
    ASSERT_EQ(s.auth_mode, LL_AUTH_OPEN);
    ASSERT(!s.telemetry_enabled);
    /* Empty name: factory devices ship unnamed; the app falls back to
     * displaying the id (MAC suffix) until the user renames. */
    ASSERT_EQ(strlen(s.name), 0);
}

void suite_nvs_logic(void)
{
    RUN(serialize_populates_schema_version);
    RUN(serialize_copies_all_fields);
    RUN(roundtrip_preserves_all_fields);
    RUN(blob_size_matches_compile_time_constant);

    RUN(null_data_returns_false);
    RUN(wrong_size_returns_false);
    RUN(schema_mismatch_returns_false);
    RUN(schema_zero_returns_false);

    RUN(brightness_clamped_to_100);
    RUN(zero_led_count_becomes_default);
    RUN(oversized_led_count_becomes_default);
    RUN(invalid_auth_mode_becomes_open);
    RUN(empty_pattern_id_becomes_solid);
    RUN(non_null_terminated_pattern_id_is_truncated);
    RUN(upper_byte_of_color_ignored);

    RUN(empty_name_survives_roundtrip);
    RUN(non_null_terminated_name_is_truncated);

    RUN(defaults_are_shipping_values);
}
