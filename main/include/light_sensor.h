#pragma once

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"

#define ADC_CHANNEL ADC_CHANNEL_5  // GPIO2 for LDR
#define ADC_UNIT ADC_UNIT_2        // ADC2

// Functions for using the LDR sensor
void ldr_init(void);
int ldr_read_raw(void);
float ldr_read_voltage(void);