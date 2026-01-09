#include "soil_moisture.h"
#include "driver/adc.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#define TAG "SOIL"
#define SOIL_ADC_CHANNEL ADC1_CHANNEL_4   // GPIO32

static adc_oneshot_unit_handle_t adc_handle;
static adc_cali_handle_t cali_handle;

static int map_value(int x, int in_min, int in_max, int out_min, int out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void soil_sensor_init(void)
{
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    adc_oneshot_new_unit(&init_config, &adc_handle);

    adc_oneshot_chan_cfg_t channel_config = {
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_12,
    };
    adc_oneshot_config_channel(adc_handle, SOIL_ADC_CHANNEL, &channel_config);

    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_12,
    };
    adc_cali_create_scheme_line_fitting(&cali_config, &cali_handle);

    ESP_LOGI(TAG, "Soil sensor initialized");
}

soil_data_t soil_sensor_read(void)
{
    soil_data_t data;
    data.raw = 0;
    data.voltage_mv = 0;
    data.moisture_percent = 0;

    adc_oneshot_read(adc_handle, SOIL_ADC_CHANNEL, &data.raw);

    adc_cali_raw_to_voltage(cali_handle, data.raw, &data.voltage_mv);

    int percent = map_value(data.raw, 4095, 1200, 0, 100);
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    data.moisture_percent = percent;

    return data;
}
