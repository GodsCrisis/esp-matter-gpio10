

if(NOT DEFINED ENV{IDF_PATH})
    message(FATAL_ERROR "IDF_PATH environment variable not set. Please run 'source ~/esp/esp-idf/export.sh'")
endif()


set(CHIP_PATH $ENV{ESP_MATTER_PATH}/connectedhomeip/connectedhomeip)
set(ESP_MATTER_COMPONENTS_PATH $ENV{ESP_MATTER_PATH}/components)


list(APPEND EXTRA_COMPONENT_DIRS
    ${CHIP_PATH}/config/esp32/components
    ${ESP_MATTER_COMPONENTS_PATH}
)


if(EXISTS ${ESP_MATTER_PATH}/examples/common)
    list(APPEND EXTRA_COMPONENT_DIRS ${ESP_MATTER_PATH}/examples/common)
endif()


include($ENV{IDF_PATH}/tools/cmake/project.cmake)


message(STATUS "ESP_MATTER_PATH = $ENV{ESP_MATTER_PATH}")
message(STATUS "CHIP_PATH = ${CHIP_PATH}")
message(STATUS "EXTRA_COMPONENT_DIRS = ${EXTRA_COMPONENT_DIRS}")
