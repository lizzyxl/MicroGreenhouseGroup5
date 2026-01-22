#include "temp_hum_sensor.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h" 
#include "config.h"
#include "i2c_init.h"

#define TAG "AHT20"
#define AHT20_ADDR 0x38

static i2c_master_dev_handle_t s_aht20_dev = NULL;

esp_err_t aht20_init(void) {
    i2c_bus_init();
    
    i2c_master_bus_handle_t bus = i2c_bus_get_handle();
    
    // Create AHT20 device on shared bus
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = AHT20_ADDR,
        .scl_speed_hz = 100000
    };
    
    esp_err_t ret = i2c_master_bus_add_device(bus, &dev_config, &s_aht20_dev);
    
    // AHT20 soft reset
    uint8_t reset_cmd[] = {0xBA};
    ret = i2c_master_transmit(s_aht20_dev, reset_cmd, 1, 1000);
    vTaskDelay(pdMS_TO_TICKS(20));
    
    ESP_LOGI(TAG, "AHT20 initialized");
    return ret;
}

esp_err_t aht20_read(float *temp, float *humidity) {
    
    // Trigger measurement
    uint8_t trigger[] = {0xAC, 0x33, 0x00};
    i2c_master_transmit(s_aht20_dev, trigger, 3, 1000);

    vTaskDelay(pdMS_TO_TICKS(80));
    
    // Read 7 bytes
    uint8_t raw_data[7];
    i2c_master_receive(s_aht20_dev, raw_data, 7, 1000);
    
    // Parse
    uint32_t hum_raw = (raw_data[1] << 12) | (raw_data[2] << 4) | (raw_data[3] >> 4);
    uint32_t temp_raw = ((raw_data[3] & 0x0F) << 16) | (raw_data[4] << 8) | raw_data[5];
    
    *humidity = (hum_raw * 100.0f) / 1048576.0f;
    *temp = ((temp_raw * 200.0f) / 1048576.0f) - 50.0f;
    
    return ESP_OK;
}