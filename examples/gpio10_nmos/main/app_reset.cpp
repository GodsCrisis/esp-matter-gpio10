#include <esp_log.h>
#include <esp_matter.h>
#include <iot_button.h>
#include <app_reset.h>

static const char *TAG = "app_reset";

#define RESET_BUTTON_GPIO GPIO_NUM_9  // BOOT button on ESP32-H2
#define FACTORY_RESET_TIMEOUT_MS 10000 // Hold for 10 seconds

static void app_reset_button_callback(void *arg, void *data)
{
    ESP_LOGI(TAG, "Factory reset triggered!");
    esp_matter::factory_reset();
}

esp_err_t app_reset_button_register()
{
    button_config_t button_config = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = RESET_BUTTON_GPIO,
            .active_level = 0,  
        },
    };

    button_handle_t button_handle = iot_button_create(&button_config);
    if (!button_handle) {
        ESP_LOGE(TAG, "Failed to create button");
        return ESP_FAIL;
    }

    esp_err_t err = iot_button_register_cb(button_handle, BUTTON_LONG_PRESS_START,
                                          app_reset_button_callback, (void *)FACTORY_RESET_TIMEOUT_MS);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register button callback");
        return err;
    }

    ESP_LOGI(TAG, "Reset button registered (GPIO%d, hold 10s for factory reset)", RESET_BUTTON_GPIO);
    return ESP_OK;
}
