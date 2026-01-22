#ifndef TEMP_HUM_SENSOR_H
#define TEMP_HUM_SENSOR_H

#include "esp_check.h"

esp_err_t aht20_init(void);
esp_err_t aht20_read(float *temp, float *humidity);

#endif