cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME "gpio10_nmos")


include($ENV{IDF_PATH}/tools/cmake/project.cmake)


include($ENV{ESP_MATTER_PATH}/esp_matter.cmake)


set(EXTRA_COMPONENT_DIRS
    "${ESP_MATTER_PATH}/examples/common"
)


project(${PROJECT_NAME})
