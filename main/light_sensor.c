#include <stdio.h>
#include "light_sensor.h"

static adc_oneshot_unit_handle_t adc_handle;
static adc_cali_handle_t adc_cali_handle;
static bool cali_enabled = false;

void ldr_init(void)
{
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    adc_oneshot_new_unit(&init_config, &adc_handle);

    adc_oneshot_chan_cfg_t chan_config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_11,
    };
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_3, &chan_config);

    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_12,
    };
    if (adc_cali_create_scheme_line_fitting(&cali_config, &adc_cali_handle) == ESP_OK) {
        cali_enabled = true;
    }
}

int ldr_read_raw(void) {
    int raw = 0;
    if (adc_oneshot_read(adc_handle, ADC_CHANNEL_3, &raw) == ESP_OK) {
        return raw;
    }
    return -1;
}

float ldr_read_voltage(void)
{
    int raw = ldr_read_raw();
    if (raw < 0) {
        return -1.0f; // Error reading raw value
    }

    if (cali_enabled) {
        int voltage;
        if (adc_cali_raw_to_voltage(adc_cali_handle, raw, &voltage) == ESP_OK) {
            return voltage / 1000.0f; // Convert mV to V
        }
    }
    return (raw / 4095.0f) * 3.3f; // Fallback calculation
}