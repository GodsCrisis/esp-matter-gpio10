idf_component_register(
    SRCS 
        "app_main.cpp"
        "app_driver.cpp"

    INCLUDE_DIRS 
        "."
    PRIV_REQUIRES 
        chip
        esp_matter
        esp_matter_console

        nvs_flash
        driver
)
