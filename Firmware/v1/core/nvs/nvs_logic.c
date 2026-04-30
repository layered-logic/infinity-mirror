#include "nvs_logic.h"

#include <string.h>

#define LL_NVS_LED_COUNT_MAX 1024u

void ll_nvs_serialize(const ll_state_t *in, ll_nvs_blob_t *out)
{
    memset(out, 0, sizeof(*out));

    out->schema_version    = LL_NVS_SCHEMA_VERSION;
    out->on                = in->on ? 1 : 0;
    out->brightness        = in->brightness;
    out->base_color_rgb    = in->base_color_rgb;
    out->led_count         = in->led_count;
    out->auth_mode         = (uint8_t)in->auth_mode;
    out->telemetry_enabled = in->telemetry_enabled ? 1 : 0;

    strncpy(out->pattern_id, in->pattern_id, sizeof(out->pattern_id) - 1);
    out->pattern_id[sizeof(out->pattern_id) - 1] = '\0';

    strncpy(out->name, in->name, sizeof(out->name) - 1);
    out->name[sizeof(out->name) - 1] = '\0';
}

bool ll_nvs_deserialize(const void *data, size_t size, ll_state_t *out)
{
    if (data == NULL || size != sizeof(ll_nvs_blob_t)) {
        return false;
    }

    ll_nvs_blob_t blob;
    memcpy(&blob, data, sizeof(blob));

    if (blob.schema_version != LL_NVS_SCHEMA_VERSION) {
        return false;
    }

    ll_state_t s = {0};

    s.on = blob.on != 0;

    s.brightness = blob.brightness > 100 ? 100 : blob.brightness;

    s.base_color_rgb = blob.base_color_rgb & 0x00FFFFFFu;

    if (blob.led_count == 0 || blob.led_count > LL_NVS_LED_COUNT_MAX) {
        s.led_count = 32;
    } else {
        s.led_count = blob.led_count;
    }

    if (blob.auth_mode == (uint8_t)LL_AUTH_PAIRED) {
        s.auth_mode = LL_AUTH_PAIRED;
    } else {
        s.auth_mode = LL_AUTH_OPEN;
    }

    s.telemetry_enabled = blob.telemetry_enabled != 0;

    /* Force null-terminate defensively, then check for empty. */
    char pid[sizeof(s.pattern_id)];
    memcpy(pid, blob.pattern_id, sizeof(pid) - 1);
    pid[sizeof(pid) - 1] = '\0';
    if (pid[0] == '\0') {
        strncpy(s.pattern_id, "solid", sizeof(s.pattern_id));
    } else {
        memcpy(s.pattern_id, pid, sizeof(s.pattern_id));
    }

    /* Name: defensively null-terminate at the buffer boundary. Empty is
     * a legal value (signals "fall back to id"), so don't substitute. */
    memcpy(s.name, blob.name, sizeof(s.name) - 1);
    s.name[sizeof(s.name) - 1] = '\0';

    *out = s;
    return true;
}
