#include "ota.h"

#include "esp_app_desc.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "ota";

esp_err_t ll_ota_init(void)
{
    /* Nothing to do upfront — esp_https_ota is request-driven. */
    return ESP_OK;
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
