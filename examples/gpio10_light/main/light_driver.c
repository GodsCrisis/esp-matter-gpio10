// light_driver.c
#include "light_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <stdio.h>

static const char *TAG = "gpio10_light";


#define OUTPUT_GPIO GPIO_NUM_10


#define LEDC_TIMER          LEDC_TIMER_0
#define LEDC_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL        LEDC_CHANNEL_0
#define LEDC_DUTY_RES       LEDC_TIMER_10_BIT // resolution of PWM duty (0..1023)
#define LEDC_FREQUENCY      20000             // 20 kHz PWM frequency


#define VOLTAGE_MIN_V 1.10f
#define VOLTAGE_MAX_V 2.00f
#define VCC 3.3f


static uint8_t current_level = 0; 
static bool current_on = false;
static TimerHandle_t on_timer = NULL;

static void set_pwm_duty_from_level(uint8_t level)
{
    
    float fraction = 0.0f;
    if (level > 0) fraction = (float)level / 254.0f;
    float vtarget = VOLTAGE_MIN_V + fraction * (VOLTAGE_MAX_V - VOLTAGE_MIN_V);


    float duty_frac = vtarget / VCC;
    if (duty_frac < 0.0f) duty_frac = 0.0f;
    if (duty_frac > 1.0f) duty_frac = 1.0f;

    uint32_t max_duty = (1 << LEDC_DUTY_RES) - 1;
    uint32_t duty = (uint32_t)(duty_frac * (float)max_duty + 0.5f);

    ESP_LOGI(TAG, "level=%d -> vtarget=%.3f V -> duty_frac=%.3f -> duty=%u", level, vtarget, duty_frac, duty);

    
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

static void on_timer_cb(TimerHandle_t xTimer)
{
    
    ESP_LOGI(TAG, "on_timer expired, turning off");
    current_on = false;
    current_level = 0;
    set_pwm_duty_from_level(0);
}



void light_driver_init(void)
{
    ESP_LOGI(TAG, "Initializing light driver on GPIO%d", OUTPUT_GPIO);

    
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz = LEDC_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    
    ledc_channel_config_t ledc_channel = {
        .gpio_num = OUTPUT_GPIO,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&ledc_channel);

    
    if (on_timer == NULL) {
        on_timer = xTimerCreate("on_timer", pdMS_TO_TICKS(1000), pdFALSE, NULL, on_timer_cb);
    }

    // Ensure output low
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);

    current_level = 0;
    current_on = false;
    ESP_LOGI(TAG, "Light driver initialized.");
}

void light_driver_set_power(bool on)
{
    ESP_LOGI(TAG, "Set power: %s", on ? "ON" : "OFF");
    current_on = on;
    if (!on) {
        
        current_level = 0;
        set_pwm_duty_from_level(0);
        if (on_timer) {
            xTimerStop(on_timer, 0);
        }
    } else {
        
        if (current_level == 0) {
            current_level = 127;
        }
        set_pwm_duty_from_level(current_level);
    }
}

void light_driver_set_level(uint8_t level)
{
    if (level > 254) level = 254;
    current_level = level;

    ESP_LOGI(TAG, "Set level: %u", level);
    if (current_on || level > 0) {
       
        current_on = (level > 0);
        set_pwm_duty_from_level(level);
    } else {
        
        light_driver_set_power(false);
    }
}

void light_driver_set_on_timer(uint32_t seconds)
{
    ESP_LOGI(TAG, "Set on timer: %us", seconds);
    if (on_timer == NULL) {
        ESP_LOGW(TAG, "on_timer not created");
        return;
    }
    if (seconds == 0) {
        xTimerStop(on_timer, 0);
        return;
    }
    
    if (seconds < 3) seconds = 3;
    if (seconds > 7) seconds = 7;

   
    xTimerStop(on_timer, 0);
    xTimerChangePeriod(on_timer, pdMS_TO_TICKS(seconds * 1000), 0);
    xTimerStart(on_timer, 0);
}
