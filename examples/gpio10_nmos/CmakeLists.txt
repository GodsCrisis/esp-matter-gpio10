
cmake_minimum_required(VERSION 3.5)


include($ENV{ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/config/esp32/components/chip/CMakeLists.txt)
include($ENV{ESP_MATTER_PATH}/esp_matter.cmake)

set(EXTRA_COMPONENT_DIRS
    "${ESP_MATTER_PATH}/examples/common"
    "${ESP_MATTER_PATH}/components"
    "${CMAKE_CURRENT_LIST_DIR}/../../components"
)

project(gpio10_nmos)

idf_build_set_property(PARTITION_TABLE_FILE ${CMAKE_CURRENT_LIST_DIR}/partitions.csv APPEND)
