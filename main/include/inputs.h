#ifndef INPUTS_H
#define INPUTS_H

#include <stdbool.h>
#include "driver/gpio.h"

// Public functions
void inputs_init(void);

extern volatile bool blue_button_pressed;
extern volatile bool white_button_pressed;

#endif
