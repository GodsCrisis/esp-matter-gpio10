#pragma once
#include <stdint.h>
#include <stdbool.h>


void light_driver_init(void);

void light_driver_set_power(bool on);

void light_driver_set_level(uint8_t level);

void light_driver_set_on_timer(uint32_t seconds);
