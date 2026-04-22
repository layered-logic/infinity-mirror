/*
 * WS2812B driver over ESP-IDF RMT.
 *
 * Wire format (WS2812B, 800 kHz, NZR):
 *   bit '0'   : T0H ~0.4 µs high, T0L ~0.85 µs low
 *   bit '1'   : T1H ~0.8 µs high, T1L ~0.45 µs low
 *   reset/latch: ≥ 50 µs low
 *
 * RMT resolution is 10 MHz (100 ns/tick), so:
 *   T0H=4 T0L=8   -> 0.4 / 0.8 µs
 *   T1H=8 T1L=4   -> 0.8 / 0.4 µs
 *   RESET: 500 ticks low = 50 µs
 *
 * We compose two encoders: a bytes_encoder that turns each payload byte
 * into the NZR bit stream, then a copy_encoder that emits the reset
 * symbol afterwards. The outer encoder below drives them in sequence.
 */

#include "led_driver.h"
#include "led_driver_logic.h"

#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "driver/rmt_tx.h"
#include "esp_check.h"
#include "esp_log.h"

static const char *TAG = "led_driver";

#define LED_RMT_RESOLUTION_HZ   10000000   /* 10 MHz -> 100 ns/tick */
#define LED_RMT_MEM_BLOCK_SYM   64         /* min for single channel */

/* Bit timings in RMT ticks at 10 MHz. */
#define T0H_TICKS  4
#define T0L_TICKS  8
#define T1H_TICKS  8
#define T1L_TICKS  4
#define RESET_TICKS  500   /* 50 µs low */

typedef struct {
    rmt_encoder_t base;
    rmt_encoder_t *bytes_encoder;
    rmt_encoder_t *copy_encoder;
    int state;                       /* 0: bytes, 1: reset, done when >=2 */
    rmt_symbol_word_t reset_symbol;
} ws2812_encoder_t;

static size_t ws2812_encode(rmt_encoder_t *enc,
                            rmt_channel_handle_t channel,
                            const void *primary_data, size_t data_size,
                            rmt_encode_state_t *ret_state)
{
    ws2812_encoder_t *self = __containerof(enc, ws2812_encoder_t, base);
    rmt_encode_state_t session_state = RMT_ENCODING_RESET;
    rmt_encode_state_t state = RMT_ENCODING_RESET;
    size_t encoded = 0;

    if (self->state == 0) {
        encoded += self->bytes_encoder->encode(self->bytes_encoder, channel,
                                               primary_data, data_size,
                                               &session_state);
        if (session_state & RMT_ENCODING_COMPLETE) {
            self->state = 1;
        }
        if (session_state & RMT_ENCODING_MEM_FULL) {
            state |= RMT_ENCODING_MEM_FULL;
            goto out;
        }
    }

    if (self->state == 1) {
        encoded += self->copy_encoder->encode(self->copy_encoder, channel,
                                              &self->reset_symbol,
                                              sizeof(self->reset_symbol),
                                              &session_state);
        if (session_state & RMT_ENCODING_COMPLETE) {
            self->state = 2;
            state |= RMT_ENCODING_COMPLETE;
        }
        if (session_state & RMT_ENCODING_MEM_FULL) {
            state |= RMT_ENCODING_MEM_FULL;
        }
    }

out:
    *ret_state = state;
    return encoded;
}

static esp_err_t ws2812_reset_encoder(rmt_encoder_t *enc)
{
    ws2812_encoder_t *self = __containerof(enc, ws2812_encoder_t, base);
    rmt_encoder_reset(self->bytes_encoder);
    rmt_encoder_reset(self->copy_encoder);
    self->state = 0;
    return ESP_OK;
}

static esp_err_t ws2812_del_encoder(rmt_encoder_t *enc)
{
    ws2812_encoder_t *self = __containerof(enc, ws2812_encoder_t, base);
    if (self->bytes_encoder) rmt_del_encoder(self->bytes_encoder);
    if (self->copy_encoder)  rmt_del_encoder(self->copy_encoder);
    free(self);
    return ESP_OK;
}

static esp_err_t new_ws2812_encoder(rmt_encoder_t **out_enc)
{
    ws2812_encoder_t *self = calloc(1, sizeof(*self));
    if (!self) return ESP_ERR_NO_MEM;

    self->base.encode = ws2812_encode;
    self->base.reset  = ws2812_reset_encoder;
    self->base.del    = ws2812_del_encoder;

    rmt_bytes_encoder_config_t bytes_cfg = {
        .bit0 = { .level0 = 1, .duration0 = T0H_TICKS,
                  .level1 = 0, .duration1 = T0L_TICKS },
        .bit1 = { .level0 = 1, .duration0 = T1H_TICKS,
                  .level1 = 0, .duration1 = T1L_TICKS },
        .flags.msb_first = 1,
    };
    esp_err_t err = rmt_new_bytes_encoder(&bytes_cfg, &self->bytes_encoder);
    if (err != ESP_OK) goto fail;

    rmt_copy_encoder_config_t copy_cfg = {0};
    err = rmt_new_copy_encoder(&copy_cfg, &self->copy_encoder);
    if (err != ESP_OK) goto fail;

    self->reset_symbol = (rmt_symbol_word_t){
        .level0 = 0, .duration0 = RESET_TICKS / 2,
        .level1 = 0, .duration1 = RESET_TICKS / 2,
    };

    *out_enc = &self->base;
    return ESP_OK;

fail:
    ws2812_del_encoder(&self->base);
    return err;
}

/* ---- Driver state ---- */

static rmt_channel_handle_t g_channel;
static rmt_encoder_t *g_encoder;
static uint8_t *g_grb_buf;
static uint16_t g_led_count;
static bool g_initialized;

esp_err_t ll_led_driver_init(uint16_t led_count)
{
    if (g_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    if (led_count == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    g_grb_buf = calloc((size_t)led_count * 3, 1);
    if (!g_grb_buf) {
        return ESP_ERR_NO_MEM;
    }

    rmt_tx_channel_config_t ch_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .gpio_num = LL_PIN_LED_DATA,
        .mem_block_symbols = LED_RMT_MEM_BLOCK_SYM,
        .resolution_hz = LED_RMT_RESOLUTION_HZ,
        .trans_queue_depth = 4,
    };
    esp_err_t err = rmt_new_tx_channel(&ch_cfg, &g_channel);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "rmt_new_tx_channel: %s", esp_err_to_name(err));
        goto fail;
    }

    err = new_ws2812_encoder(&g_encoder);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ws2812 encoder: %s", esp_err_to_name(err));
        goto fail;
    }

    err = rmt_enable(g_channel);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "rmt_enable: %s", esp_err_to_name(err));
        goto fail;
    }

    g_led_count = led_count;
    g_initialized = true;
    ESP_LOGI(TAG, "init: pin=%d leds=%u", LL_PIN_LED_DATA, (unsigned)led_count);
    return ESP_OK;

fail:
    if (g_encoder) { rmt_del_encoder(g_encoder); g_encoder = NULL; }
    if (g_channel) { rmt_del_channel(g_channel); g_channel = NULL; }
    free(g_grb_buf); g_grb_buf = NULL;
    return err;
}

void ll_led_driver_deinit(void)
{
    if (!g_initialized) return;
    rmt_disable(g_channel);
    rmt_del_encoder(g_encoder);
    rmt_del_channel(g_channel);
    free(g_grb_buf);
    g_encoder = NULL;
    g_channel = NULL;
    g_grb_buf = NULL;
    g_led_count = 0;
    g_initialized = false;
}

esp_err_t ll_led_driver_write(const uint8_t *rgb, uint8_t brightness_pct)
{
    if (!g_initialized) return ESP_ERR_INVALID_STATE;
    if (!rgb) return ESP_ERR_INVALID_ARG;

    led_driver_encode_frame(rgb, g_led_count, brightness_pct, g_grb_buf);

    rmt_transmit_config_t tx_cfg = { .loop_count = 0 };
    esp_err_t err = rmt_transmit(g_channel, g_encoder,
                                 g_grb_buf, (size_t)g_led_count * 3, &tx_cfg);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "rmt_transmit: %s", esp_err_to_name(err));
        return err;
    }
    return rmt_tx_wait_all_done(g_channel, portMAX_DELAY);
}

esp_err_t ll_led_driver_fill(uint8_t r, uint8_t g, uint8_t b,
                             uint8_t brightness_pct)
{
    if (!g_initialized) return ESP_ERR_INVALID_STATE;

    /* Temp RGB buffer on stack would blow stack for large counts; use a
     * small heap buffer instead. 32 LEDs = 96 bytes today, but scales. */
    uint8_t *tmp = malloc((size_t)g_led_count * 3);
    if (!tmp) return ESP_ERR_NO_MEM;

    for (uint16_t i = 0; i < g_led_count; i++) {
        tmp[i * 3 + 0] = r;
        tmp[i * 3 + 1] = g;
        tmp[i * 3 + 2] = b;
    }
    esp_err_t err = ll_led_driver_write(tmp, brightness_pct);
    free(tmp);
    return err;
}

esp_err_t ll_led_driver_clear(void)
{
    return ll_led_driver_fill(0, 0, 0, 0);
}
