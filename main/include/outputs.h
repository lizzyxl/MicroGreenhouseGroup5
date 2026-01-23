#ifndef OUTPUTS_H
#define OUTPUTS_H

#include <stdbool.h>
#include "driver/gpio.h"

typedef enum {
    LED_OFF,
    LED_ON
} led_state;

// Public functions
void outputs_init(void);
void set_red_connection_led(led_state state);
void set_green_moisture_led(led_state state);

#endif
