#ifndef DISPLAY_H
#define DISPLAY_H

#include "utils.h"

void greenhouse_display_init(void);
void display_draw(measurements_t *measurements, bool *button_press);

#endif