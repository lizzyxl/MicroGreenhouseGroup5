#ifndef TEMP_HUM_SENSOR_H
#define TEMP_HUM_SENSOR_H

#define I2C_MASTER_FREQ_HZ 100000     /*!< I2C master clock frequency */

#include "i2c_bus.h"  

void aht20_init();
void aht20_read(float *temperature, float *humidity);

#endif