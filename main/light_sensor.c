#include <stdio.h>
#include "light_sensor.h"

static adc_oneshot_unit_handle_t adc_handle;
static adc_cali_handle_t adc_cali_handle;
static bool cali_enabled = false;

// Initialize the LDR sensor
void ldr_init(void)
{
    // Configure ADC unit (ADC2 for GPIO2)
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_2,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    adc_oneshot_new_unit(&init_config, &adc_handle);

    // Configure ADC channel settings
    adc_oneshot_chan_cfg_t chan_config = {
        .bitwidth = ADC_BITWIDTH_12, // 12-bit resolution (0-4095)
        .atten = ADC_ATTEN_DB_11,
    };
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_3, &chan_config);

    // Configure calibration (maps raw ADC values to voltage)
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_11,
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
    // Try to read the raw ADC value fom channel 2 (GPIO2)
    if (adc_oneshot_read(adc_handle, ADC_CHANNEL_2, &raw) == ESP_OK)
    {
        return raw;
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