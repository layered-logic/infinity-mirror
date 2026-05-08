#include "ota.h"

#include <stdbool.h>

#include "esp_app_desc.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "state_bus.h"
#include "state_bus_events.h"

static const char *TAG = "ota";
static bool g_marked_valid;

esp_err_t ll_ota_init(void)
{
    /* Nothing to do upfront — esp_https_ota is request-driven. */
    return ESP_OK;
}

/*
 * Mark the running OTA image valid on the first LL_EV_WIFI_CONNECTED
 * after boot. If the image was installed via OTA, it lives in
 * PENDING_VERIFY state until this call lands; the bootloader uses
 * "did the new image successfully run?" as a proxy for "should it be
 * promoted permanently?" Reaching the network is our health proxy —
 * if we got there, the firmware can do its primary job.
 *
 * On failures other than "not OTA-pending" we log but don't fail —
 * the next WIFI_CONNECTED will retry. Idempotent.
 */
static void mark_valid_on_first_connect(void *arg, esp_event_base_t base,
                                         int32_t id, void *data)
{
    (void)arg; (void)base; (void)id; (void)data;
    if (g_marked_valid) {
        return;
    }

    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t state = ESP_OTA_IMG_UNDEFINED;
    if (esp_ota_get_state_partition(running, &state) == ESP_OK
        && state == ESP_OTA_IMG_PENDING_VERIFY) {
        esp_err_t err = esp_ota_mark_app_valid_cancel_rollback();
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "OTA image marked valid — rollback canceled");
        } else {
            ESP_LOGW(TAG, "mark_app_valid: %s", esp_err_to_name(err));
            return;  /* try again on next WIFI_CONNECTED */
        }
    } else {
        /* Factory image, already-valid OTA, or rollback disabled —
         * nothing to mark, but record that we've inspected so we don't
         * keep poking on every reconnect. */
        ESP_LOGD(TAG, "image state=%d, no mark needed", (int)state);
    }
    g_marked_valid = true;
}

esp_err_t ll_ota_subscribe(void)
{
    return esp_event_handler_instance_register_with(
        ll_state_bus_get_loop(),
        LL_STATE_EVENT_BASE, LL_EV_WIFI_CONNECTED,
        mark_valid_on_first_connect, NULL, NULL);
}

esp_err_t ll_ota_start(const char *url)
{
    if (!url || !url[0]) return ESP_ERR_INVALID_ARG;

    const esp_app_desc_t *running = esp_app_get_description();
    ESP_LOGI(TAG, "OTA start: from %s, current fw=%s", url, running->version);

    esp_http_client_config_t http_cfg = {
        .url = url,
        .timeout_ms = 30000,
        .keep_alive_enable = true,
        /* Dev-test path uses plain HTTP, but if a caller passes https://
         * we still skip CN check rather than baking in a CA bundle that
         * we don't trust for this throwaway scope. */
        .skip_cert_common_name_check = true,
    };
    esp_https_ota_config_t cfg = {
        .http_config = &http_cfg,
    };

    esp_err_t ret = esp_https_ota(&cfg);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "OTA download + verify OK; rebooting into new image");
        /* Give the log line a chance to drain before the reset cuts UART. */
        vTaskDelay(pdMS_TO_TICKS(500));
        esp_restart();
    }
    ESP_LOGE(TAG, "OTA failed: %s", esp_err_to_name(ret));
    return ret;
}
