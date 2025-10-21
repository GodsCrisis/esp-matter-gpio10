// app_main.cpp
#include "light_driver.h"
#include "esp_log.h"
#include "esp_matter.h"     
#include "chip_porting.h"   

static const char *TAG = "gpio10_app";



extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting gpio10_light example");

    
    light_driver_init();



    ESP_LOGI(TAG, "app_main done. Matter stack should handle pairing. Use Google Home to commission.");
}
