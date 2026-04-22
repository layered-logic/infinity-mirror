#pragma once

/*
 * Pure-C NVS logic: schema versioning, serialization, sanitization.
 *
 * Split from nvs.c so it builds on the host for unit tests. nvs.c owns the
 * ESP-IDF nvs_flash plumbing and the debounced-save timer; everything that
 * turns ll_state_t into bytes (and vice versa) lives here.
 *
 * Schema version bump rules (firmware-spec.md §4):
 *   - Bump LL_NVS_SCHEMA_VERSION whenever ll_nvs_blob_t layout changes.
 *   - On load, mismatched schema → discard blob, fall back to defaults.
 *     No migration path until we have real shipped users.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "state_bus.h"

#define LL_NVS_SCHEMA_VERSION 1u

/*
 * On-flash layout for ll_settings:state. Packed so sizeof() is the wire
 * size across compilers. Field order matches ll_state_t semantically but
 * isn't a memcpy — we translate field by field so struct padding drift
 * can't silently corrupt old blobs.
 */
typedef struct __attribute__((packed)) {
    uint16_t schema_version;
    uint8_t  on;
    uint8_t  brightness;
    uint32_t base_color_rgb;
    uint16_t led_count;
    uint8_t  auth_mode;
    uint8_t  telemetry_enabled;
    char     pattern_id[32];
} ll_nvs_blob_t;

/*
 * Pack ll_state_t into the on-flash blob form. Always succeeds; fills
 * schema_version with the current compiled value.
 */
void ll_nvs_serialize(const ll_state_t *in, ll_nvs_blob_t *out);

/*
 * Attempt to unpack an on-flash blob into ll_state_t. Returns true on
 * success.
 *
 * Hard failures (out left untouched, caller should use defaults):
 *   - data == NULL
 *   - size != sizeof(ll_nvs_blob_t)
 *   - schema_version != LL_NVS_SCHEMA_VERSION
 *
 * Soft validation (always applied on success, doesn't fail the load):
 *   - brightness clamped to 0-100
 *   - led_count of 0 or > 1024 → default
 *   - auth_mode outside {OPEN, PAIRED} → OPEN
 *   - pattern_id null-terminated at [31]; empty → "solid"
 *
 * The soft validation protects us from field-level drift where an old
 * firmware wrote a weird value; we recover instead of wiping everything.
 */
bool ll_nvs_deserialize(const void *data, size_t size, ll_state_t *out);
