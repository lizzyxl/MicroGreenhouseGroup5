#ifndef INPUTS_H
#define INPUTS_H

#include <stdbool.h>
#include "driver/gpio.h"

// Public functions
void inputs_init(void);

bool input_button_pressed_white(void);
bool input_button_pressed_blue(void);

#endif
