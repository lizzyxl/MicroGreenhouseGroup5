#ifndef GROW_LIGHT_H
#define GROW_LIGHT_H

void grow_light_init(void);
void grow_light_control(int light_level);
void grow_light_task(void *pvParameters);

#endif
