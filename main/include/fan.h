#ifndef FAN_H
#define FAN_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"

void fan_init(void);
void fan_control(float temperature, float temp_higher_treshold, float temp_lower_treshold);

#endif