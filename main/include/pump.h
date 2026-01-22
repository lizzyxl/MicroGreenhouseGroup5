#ifndef PUMP_H
#define PUMP_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "outputs.h"
#include "parameter_config.h"

// Public API
void pump_init(void);
void pump_control(float moisture, greenhouse_config_t greenhouse_config);

#endif