#include <stdio.h>
#include "light_sensor.h"
#include "config.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_log.h"
#include "adc_init.h"

#define TAG "LIGHT_SENSOR"

static adc_cali_handle_t adc_cali_handle;
static bool cali_enabled = false;

// Initialize the LDR sensor
void ldr_init(void)
{
    adc_init_all();
    // Configure calibration (maps raw ADC values to voltage)
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = LDR_ADC_UNIT,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    // If calibration is available, enable it
    if (adc_cali_create_scheme_line_fitting(&cali_config, &adc_cali_handle) == ESP_OK)
    {
        cali_enabled = true;
    }
}

// Read raw ADC value from LDR
int ldr_read_raw(void)
{
    int raw = 0;
    if (adc_oneshot_read(get_adc1_handle(), LDR_ADC_CHANNEL, &raw) == ESP_OK)
    {
        return raw;
    }
    return -1; // return -1 if reading failed
}

// Read raw ADC value from LDR
float ldr_read_percent(void)
{   
    int raw = 0;
    if (adc_oneshot_read(get_adc1_handle(), LDR_ADC_CHANNEL, &raw) == ESP_OK)
    {
        return raw / 4095.0f * 100.0f;
    }
    return -1; // return -1 if reading failed
}

// Convert raw ADC value into voltage (V)
float ldr_read_voltage(void)
{
    int raw = ldr_read_raw();
    if (raw < 0)
    {
        return -1.0f; // Error reading raw value
    }

    // If calibration is enables, use it for accurate voltage
    if (cali_enabled)
    {
        int voltage;
        if (adc_cali_raw_to_voltage(adc_cali_handle, raw, &voltage) == ESP_OK)
        {
            return voltage / 1000.0f; // Convert mV to V
        }
    }
    // Fallback to simple calculation if calibration is not available
    return (raw / 4095.0f) * 3.3f; // Scale raw value to voltage (0-3.3V)
}