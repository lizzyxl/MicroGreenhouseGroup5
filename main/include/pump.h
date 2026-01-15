#ifndef PUMP_H
#define PUMP_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"

// Config
#define PUMP_GPIO GPIO_NUM_15 //GPIO_NUM_39 in real project
#define MOISTURE_LED GPIO_NUM_17 //GPIO_NUM_37 in real project
#define PUMP_MOIST_LOWER_THRESHOLD 30.0f //in percent of soil moisture, treshold that starts the pump
#define PUMP_MOIST_HIGHER_THRESHOLD 50.0f //in percent of soil moisture, treshold that stops the pump
#define RESERVOIR_EMPTY_TIMEOUT 60000 // time after the moisture has not changed significantly, indicating an empty reservoir
#define RESERVOIR_EMPTY_TRESHOLD 5.0f //percent of moisture change that needs to happen
#define BLINK_INTERVAL_MS 1000 //blink intervall of led
#define PUMP_CHECK_TIME 1000 //ms how often the pump should check for the current moisture level and activate/deactivate

typedef enum {
    LED_OFF,
    LED_ON,
    LED_BLINKING
} led_state;

// Public API
void pump_init(void);
led_state pump_control(float moisture);
void moisture_led_control(led_state moisture_led_state);

#endif