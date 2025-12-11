#pragma once

typedef struct {
    int raw;
    int voltage_mv;
    int moisture_percent;
} soil_data_t;

void soil_sensor_init(void);
soil_data_t soil_sensor_read(void);
