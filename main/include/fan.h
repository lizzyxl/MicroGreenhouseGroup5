#ifndef FAN_H
#define FAN_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"

// Config
#define FAN_GPIO GPIO_NUM_13
#define FAN_TEMP_HIGHER_THRESHOLD 25.0f
#define FAN_TEMP_LOWER_THRESHOLD 20.0f
#define FAN_CHECK_INTERVAL_MS 1000

typedef struct {
     float temperature;
} temp_status_t;

bool fan_init(void);
void fan_create_task(void);

extern QueueHandle_t temp_queue;
extern volatile bool fan_state;

#endif