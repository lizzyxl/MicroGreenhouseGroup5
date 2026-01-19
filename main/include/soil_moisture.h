#ifndef SOIL_MOISTURE_H
#define SOIL_MOISTURE_H

typedef struct {
    int raw;
    int voltage_mv;
    int moisture_percent;
} soil_data_t;

void soil_sensor_init(void);
int soil_sensor_read(void);

#endif