#ifndef PUMP_H
#define PUMP_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "outputs.h"

// Public API
void pump_init(void);
led_state pump_control(float moisture, float soilmoist_lower_treshold_pct, float soilmoist_higher_treshold_pct);

#endif