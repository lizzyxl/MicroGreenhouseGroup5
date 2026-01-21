#ifndef PUMP_H
#define PUMP_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "outputs.h"

// Public API
void pump_init(void);
void pump_control(float moisture, float soilmoist_threshold_pct);

#endif