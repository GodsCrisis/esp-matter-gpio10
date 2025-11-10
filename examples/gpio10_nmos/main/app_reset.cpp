#include <esp_log.h>
#include <esp_matter.h>
#include <iot_button.h>
#include <button_gpio.h>
#include <app_reset.h>
#include <driver/gpio.h>

static const char *TAG = "app_reset";
#define RESET_BUTTON_GPIO GPIO_NUM_9
#define FACTORY_RESET_TIMEOUT_MS 10000

static button_handle_t button_handle = NULL;

static void app_reset_button_callback(void *button_handle, void *usr_data)
{
    ESP_LOGI(TAG, "Factory reset triggered!");
    esp_matter::factory_reset();
}

esp_err_t app_reset_button_register()
{

    button_config_t button_cfg = {
        .long_press_time = FACTORY_RESET_TIMEOUT_MS,
        .short_press_time = 0,
    };
    

    button_gpio_cfg_t gpio_cfg = {
        .gpio_num = RESET_BUTTON_GPIO,
        .active_level = 0,
    };
    

    esp_err_t err = iot_button_create(&button_cfg, &button_gpio_driver, (void*)&gpio_cfg, &button_handle);
    
    if (err != ESP_OK || button_handle == NULL) {
        ESP_LOGE(TAG, "Failed to create button: %s", esp_err_to_name(err));
        return ESP_FAIL;
    }

    err = iot_button_register_cb(button_handle, BUTTON_LONG_PRESS_START, 
                                 NULL, app_reset_button_callback, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register button callback: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "Reset button registered (GPIO%d, hold 10s for factory reset)", RESET_BUTTON_GPIO);
    return ESP_OK;
}
