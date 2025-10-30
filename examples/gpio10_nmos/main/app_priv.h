#pragma once

#include <esp_err.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


#define DEFAULT_POWER false

#define ENDPOINT_FLAG_NONE 0


esp_err_t app_driver_attribute_update(uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, void *val);

#ifdef __cplusplus
}
#endif
