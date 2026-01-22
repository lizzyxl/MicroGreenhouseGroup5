#include "soil_moisture.h"
#include "driver/adc.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "config.h"
#include "adc_init.h"

#define TAG "SOIL_MOISTURE_SENSOR"

static adc_cali_handle_t cali_handle;

static int map_value(int x, int in_min, int in_max, int out_min, int out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void soil_sensor_init(void)
{
    adc_init_all();
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = SOIL_ADC_UNIT,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    adc_cali_create_scheme_line_fitting(&cali_config, &cali_handle);

    ESP_LOGI(TAG, "Soil moisture sensor initialized");
}

int soil_sensor_read(void)
{
    int raw = 0;
    int voltage_mv = 0;

    adc_oneshot_read(get_adc1_handle(), SOIL_ADC_CHANNEL, &raw);

    adc_cali_raw_to_voltage(cali_handle, raw, &voltage_mv);

    int percent = map_value(raw, 4095, 1200, 0, 100);
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    return percent;
}