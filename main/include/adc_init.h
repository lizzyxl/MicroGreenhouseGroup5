#ifndef ADC_INIT_H
#define ADC_INIT_H

#include "driver/adc.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

esp_err_t adc_init_all(void);
adc_oneshot_unit_handle_t get_adc1_handle(void);

#endif