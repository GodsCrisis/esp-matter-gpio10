#include <esp_log.h>
#include <esp_matter.h>
#include <iot_button.h>
#include <app_reset.h>
#include <driver/gpio.h>

static const char *TAG = "app_reset";

#define RESET_BUTTON_GPIO GPIO_NUM_9
#define FACTORY_RESET_TIMEOUT_MS 10000

static iot_button_handle_t button_handle = NULL;

#ifndef BUTTON_TYPE_USER
#define BUTTON_TYPE_USER 0
#endif

#ifndef BUTTON_TYPE_GPIO
#define BUTTON_TYPE_GPIO BUTTON_TYPE_USER
#endif

static void app_reset_button_callback(void *arg)
{
    ESP_LOGI(TAG, "Factory reset triggered!");
    esp_matter::factory_reset();
}

esp_err_t app_reset_button_register()
{
    ESP_LOGI(TAG, "Registering factory reset button on GPIO%d", RESET_BUTTON_GPIO);

    button_config_t button_conf = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_num = RESET_BUTTON_GPIO,
        .active_level = 0,
    };

    button_handle_t btn = iot_button_create(&button_conf);
    if (btn == NULL) {
        ESP_LOGE(TAG, "Failed to create button!");
        return ESP_FAIL;
    }
    button_handle = btn;

    esp_err_t err = iot_button_register_cb(btn, BUTTON_CB_LONGPRESS, app_reset_button_callback, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register button callback: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "Reset button configured. Hold for %d ms to trigger reset.", FACTORY_RESET_TIMEOUT_MS);
    return ESP_OK;
}
