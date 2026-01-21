#include "adc.h"
#include "esp_log.h"
#include "config.h"

static adc_oneshot_unit_handle_t adc1_handle = NULL;

esp_err_t adc_init_all(void) {
    if (adc1_handle) return ESP_OK;
    
    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_cfg, &adc1_handle));
    
    adc_oneshot_chan_cfg_t cfg = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    
    // Soil moisture
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_4, &cfg));
    // LDR
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, SOIL_ADC_CHANNEL, &cfg));
    
    return ESP_OK;
}

adc_oneshot_unit_handle_t get_adc1_handle(void) {
    return adc1_handle;
}