#ifndef FAN_H
#define FAN_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "parameter_config.h"

void fan_init(void);
void fan_control(float temperature, float humidity, greenhouse_config_t greenhouse_config);

#endif