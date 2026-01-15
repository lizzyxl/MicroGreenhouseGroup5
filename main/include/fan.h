#ifndef FAN_H
#define FAN_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"

// Config
#define FAN_TEMP_HIGHER_THRESHOLD 25.0f
#define FAN_TEMP_LOWER_THRESHOLD 20.0f

typedef struct {
     float temperature;
} temp_status_t;

void fan_init(void);
void fan_control(float temperature);

#endif