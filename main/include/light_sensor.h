#pragma once

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"

void ldr_init(void);
int ldr_read_raw(void);
float ldr_read_voltage(void);


