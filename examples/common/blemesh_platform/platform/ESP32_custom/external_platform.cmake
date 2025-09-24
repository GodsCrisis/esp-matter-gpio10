# check the source file dirs
set(EXPLANT_SRC_DIRS_LIST "${CMAKE_CURRENT_LIST_DIR}/"
                          "${CMAKE_CURRENT_LIST_DIR}/route_hook")

set(EXPLANT_INCLUDE_DIRS_LIST "${CMAKE_CURRENT_LIST_DIR}/../../"
                              "${CMAKE_CURRENT_LIST_DIR}"
                              "${CMAKE_CURRENT_LIST_DIR}/bluedroid"
                              "${CMAKE_CURRENT_LIST_DIR}/nimble"
                              "${CMAKE_CURRENT_LIST_DIR}/route_hook")

set(EXPLANT_EXCLUDE_SRCS_LIST)

if (CONFIG_USE_MINIMAL_MDNS)
    list(APPEND EXPLANT_EXCLUDE_SRCS_LIST "${CMAKE_CURRENT_LIST_DIR}/DnssdImpl.cpp")
endif()

if ((CONFIG_USE_MINIMAL_MDNS) OR ((NOT CONFIG_ENABLE_WIFI_STATION) AND (NOT CONFIG_ENABLE_WIFI_AP)))
  list(APPEND EXCLUDE_SRCS_LIST "${CMAKE_CURRENT_LIST_DIR}/WiFiDnssdImpl.cpp")
endif()

if((NOT CONFIG_ENABLE_WIFI_STATION) AND (NOT CONFIG_ENABLE_WIFI_AP))
    list(APPEND EXPLANT_EXCLUDE_SRCS_LIST "${CMAKE_CURRENT_LIST_DIR}/ConnectivityManagerImpl_WiFi.cpp"
                                          "${CMAKE_CURRENT_LIST_DIR}/NetworkCommissioningDriver.cpp"
                                          "${CMAKE_CURRENT_LIST_DIR}/route_hook/ESP32RouteHook.c"
                                          "${CMAKE_CURRENT_LIST_DIR}/route_hook/ESP32RouteTable.c")
endif()

if(NOT CONFIG_ENABLE_ROUTE_HOOK)
    list(APPEND EXPLANT_EXCLUDE_SRCS_LIST "${CMAKE_CURRENT_LIST_DIR}/route_hook/ESP32RouteHook.c"
                                          "${CMAKE_CURRENT_LIST_DIR}/route_hook/ESP32RouteTable.c")
endif()

if ((CONFIG_BT_ENABLED) AND (CONFIG_ENABLE_CHIPOBLE))
    if (CONFIG_BT_NIMBLE_ENABLED)
        list(APPEND EXPLANT_SRC_DIRS_LIST "${CMAKE_CURRENT_LIST_DIR}/nimble")
        if(NOT CONFIG_ENABLE_ESP32_BLE_CONTROLLER)
            list(APPEND EXPLANT_EXCLUDE_SRCS_LIST "${CMAKE_CURRENT_LIST_DIR}/nimble/ChipDeviceScanner.cpp"
                                                  "${CMAKE_CURRENT_LIST_DIR}/nimble/misc.c"
                                                  "${CMAKE_CURRENT_LIST_DIR}/nimble/peer.c")
        endif()
    else()
        list(APPEND EXPLANT_SRC_DIRS_LIST "${CMAKE_CURRENT_LIST_DIR}/bluedroid")
        if(NOT CONFIG_ENABLE_ESP32_BLE_CONTROLLER)
            list(APPEND EXPLANT_EXCLUDE_SRCS_LIST "${CMAKE_CURRENT_LIST_DIR}/bluedroid/ChipDeviceScanner.cpp")
        endif()
    endif()
endif()

if (NOT CONFIG_ENABLE_ETHERNET_TELEMETRY)
    list(APPEND EXPLANT_EXCLUDE_SRCS_LIST "${CMAKE_CURRENT_LIST_DIR}/ConnectivityManagerImpl_Ethernet.cpp"
                                          "${CMAKE_CURRENT_LIST_DIR}/NetworkCommissioningDriver_Ethernet.cpp")
endif()

if ((NOT CONFIG_OPENTHREAD_ENABLED) OR (NOT CONFIG_ENABLE_MATTER_OVER_THREAD))
    list(APPEND EXPLANT_EXCLUDE_SRCS_LIST "${CMAKE_CURRENT_LIST_DIR}/OpenthreadLauncher.cpp"
                                          "${CMAKE_CURRENT_LIST_DIR}/ThreadStackManagerImpl.cpp")
endif()

if (NOT CONFIG_ENABLE_OTA_REQUESTOR)
    list(APPEND EXPLANT_EXCLUDE_SRCS_LIST "${CMAKE_CURRENT_LIST_DIR}/OTAImageProcessorImpl.cpp")
endif()


if (NOT CONFIG_ENABLE_ESP32_FACTORY_DATA_PROVIDER)
    list(APPEND EXPLANT_EXCLUDE_SRCS_LIST "${CMAKE_CURRENT_LIST_DIR}/ESP32FactoryDataProvider.cpp")
endif()

if (NOT CONFIG_ENABLE_ESP32_DEVICE_INFO_PROVIDER)
    list(APPEND EXPLANT_EXCLUDE_SRCS_LIST "${CMAKE_CURRENT_LIST_DIR}/ESP32DeviceInfoProvider.cpp")
endif()

if (NOT CONFIG_SEC_CERT_DAC_PROVIDER)
    list(APPEND EXPLANT_EXCLUDE_SRCS_LIST "${CMAKE_CURRENT_LIST_DIR}/ESP32SecureCertDACProvider.cpp")
endif()

if (NOT CONFIG_USE_ESP32_ECDSA_PERIPHERAL)
    list(APPEND EXPLANT_EXCLUDE_SRCS_LIST "${CMAKE_CURRENT_LIST_DIR}/ESP32CHIPCryptoPAL.cpp")
endif()

if(CONFIG_CHIP_MEM_ALLOC_MODE_INTERNAL)
    list(APPEND EXCLUDE_SRCS_LIST "${CMAKE_CURRENT_LIST_DIR}/src/platform/ESP32/CHIPMem-PlatformExternal.cpp"
                                  "${CMAKE_CURRENT_LIST_DIR}/src/platform/ESP32/CHIPMem-PlatformDefault.cpp")
elseif(CONFIG_CHIP_MEM_ALLOC_MODE_EXTERNAL)
    list(APPEND EXCLUDE_SRCS_LIST "${CMAKE_CURRENT_LIST_DIR}/src/platform/ESP32/CHIPMem-PlatformInternal.cpp"
                                  "${CMAKE_CURRENT_LIST_DIR}/src/platform/ESP32/CHIPMem-PlatformDefault.cpp")
elseif(CONFIG_CHIP_MEM_ALLOC_MODE_DEFAULT)
    list(APPEND EXCLUDE_SRCS_LIST "${CMAKE_CURRENT_LIST_DIR}/src/platform/ESP32/CHIPMem-PlatformInternal.cpp"
                                  "${CMAKE_CURRENT_LIST_DIR}/src/platform/ESP32/CHIPMem-PlatformExternal.cpp")
endif()

if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/generate-include-files.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/generate-include-files.cmake)
else()
    message(FATAL_ERROR "There should be a generate-include-files.cmake file in CONFIG_CHIP_EXTERNAL_PLATFORM_DIR!")
endif()
