#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

// Functions for using the LDR sensor
void ldr_init(void);
int ldr_read_raw(void);
float ldr_read_voltage(void);
float ldr_read_percent(void);

#endif