#include <esp_log.h>
#include <app_priv.h>

static const char *TAG = "app_driver";

esp_err_t app_driver_attribute_update(uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, void *val)
{

    ESP_LOGI(TAG, "Driver attribute update - endpoint: %d, cluster: %ld, attribute: %ld",
             endpoint_id, cluster_id, attribute_id);
    return ESP_OK;
}
