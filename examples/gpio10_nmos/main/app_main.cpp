#include <esp_err.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include <esp_matter.h>
#include <esp_matter_console.h>
#include <esp_matter_ota.h>

#include <app_priv.h>
#include <app_reset.h>
#include <driver/ledc.h>

static const char *TAG = "app_main";

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;

constexpr gpio_num_t NMOS_GATE_GPIO = GPIO_NUM_10;
constexpr ledc_channel_t PWM_CHANNEL = LEDC_CHANNEL_0;
constexpr ledc_timer_t PWM_TIMER = LEDC_TIMER_0;
constexpr uint32_t PWM_FREQUENCY = 1000;  
constexpr ledc_timer_bit_t PWM_RESOLUTION = LEDC_TIMER_10_BIT;  

static uint16_t light_endpoint_id = 0;
static uint16_t current_level = 0;  
static bool power_state = false;

static void app_driver_nmos_init()
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = PWM_RESOLUTION,
        .timer_num = PWM_TIMER,
        .freq_hz = PWM_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .gpio_num = NMOS_GATE_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = PWM_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = PWM_TIMER,
        .duty = 0,  
        .hpoint = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    ESP_LOGI(TAG, "NMOS gate driver initialized on GPIO%d", NMOS_GATE_GPIO);
    ESP_LOGI(TAG, "PWM: %ldHz, %d-bit resolution", PWM_FREQUENCY, PWM_RESOLUTION);
}

static void app_driver_nmos_set_duty(uint16_t duty)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL);
    
    float duty_percent = (duty / 1023.0f) * 100.0f;
    ESP_LOGI(TAG, "NMOS gate PWM set to %d/1023 (%.1f%% duty)", duty, duty_percent);
}

static void app_driver_nmos_set_power(bool power)
{
    power_state = power;
    
    if (power) {
        uint16_t duty = (current_level > 0) ? current_level : 512;
        current_level = duty;
        app_driver_nmos_set_duty(duty);
        ESP_LOGI(TAG, "NMOS turned ON at %d/1023", duty);
    } else {
        app_driver_nmos_set_duty(0);
        ESP_LOGI(TAG, "NMOS turned OFF");
    }
}

static void app_driver_nmos_set_level(uint8_t level)
{
    current_level = (level * 1023) / 255;
    
    if (power_state) {
        app_driver_nmos_set_duty(current_level);
    }
    
    ESP_LOGI(TAG, "NMOS level set to %d/255 (PWM: %d/1023)", level, current_level);
}

static esp_err_t app_attribute_update_cb(attribute::callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    esp_err_t err = ESP_OK;

    if (type == PRE_UPDATE) {
        // Pre-update logic if needed
    } else {
        if (cluster_id == OnOff::Id) {
            if (attribute_id == OnOff::Attributes::OnOff::Id) {
                ESP_LOGI(TAG, "OnOff attribute update: %d", val->val.b);
                app_driver_nmos_set_power(val->val.b);
            }
        } else if (cluster_id == LevelControl::Id) {
            if (attribute_id == LevelControl::Attributes::CurrentLevel::Id) {
                uint8_t level = val->val.u8;
                ESP_LOGI(TAG, "Level Control attribute update: %d", level);
                app_driver_nmos_set_level(level);
            }
        }
    }

    return err;
}

static esp_err_t app_identification_cb(identification::callback_type_t type, uint16_t endpoint_id,
                                       uint8_t effect_id, uint8_t effect_variant, void *priv_data)
{
    ESP_LOGI(TAG, "Identification callback: type: %u, effect: %u", type, effect_id);
    return ESP_OK;
}

static void app_create_endpoint()
{
    node_t *node = node::get();
    if (!node) {
        ESP_LOGE(TAG, "Matter node not found");
        return;
    }

    // Create extended color light endpoint configuration
    extended_color_light::config_t light_config;
    light_config.on_off.on_off = DEFAULT_POWER;
    light_config.on_off.lighting.start_up_on_off = nullptr;
    
    // Set current level using nullable type (cast to uint8_t)
    light_config.level_control.current_level = static_cast<uint8_t>(0);
    light_config.level_control.start_up_current_level = nullptr;

    endpoint_t *endpoint = extended_color_light::create(node, &light_config, ENDPOINT_FLAG_NONE, NULL);
    
    if (!endpoint) {
        ESP_LOGE(TAG, "Failed to create endpoint");
        return;
    }

    light_endpoint_id = endpoint::get_id(endpoint);
    ESP_LOGI(TAG, "NMOS PWM controller endpoint created with endpoint_id %d", light_endpoint_id);

    cluster_t *cluster = cluster::get(endpoint, OnOff::Id);
    cluster::add_function_list(cluster, NULL, 0);
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    // Initialize NVS
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Initialize NMOS driver
    app_driver_nmos_init();

    // Create Matter node
    node::config_t node_config;
    node_t *node = node::create(&node_config, app_attribute_update_cb, app_identification_cb);
    if (!node) {
        ESP_LOGE(TAG, "Matter node creation failed");
        return;
    }

    // Create endpoint
    app_create_endpoint();

    // Start Matter stack (v4.x API - no callback parameter)
    err = esp_matter::start(nullptr);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Matter start failed: %d", err);
        return;
    }

    // Register reset button
    app_reset_button_register();

    ESP_LOGI(TAG, "===================================================");
    ESP_LOGI(TAG, "Matter GPIO10 NMOS PWM Controller Started");
    ESP_LOGI(TAG, "GPIO10 configured for NMOS gate drive (PWM)");
    ESP_LOGI(TAG, "Control via Matter (Google Home/ESPRainmaker)");
    ESP_LOGI(TAG, "Frequency: %ldHz, Resolution: 10-bit (0-1023)", PWM_FREQUENCY);
    ESP_LOGI(TAG, "===================================================");
}
