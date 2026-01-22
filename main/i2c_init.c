#include "i2c_init.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "config.h"
#include <driver/i2c_master.h>

#define TAG "I2C"

static i2c_master_bus_handle_t s_i2c_bus = NULL;
static bool s_initialized = false;

esp_err_t i2c_bus_init(void) {
    if (s_initialized) {
        return ESP_OK;
    }
    
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .i2c_port = I2C_BUS_PORT,
        .sda_io_num = PIN_NUM_SDA,
        .scl_io_num = PIN_NUM_SCL,
        .flags.enable_internal_pullup = true,
    };
    
    esp_err_t ret = i2c_new_master_bus(&bus_config, &s_i2c_bus);
    if (ret == ESP_OK) {
        s_initialized = true;
        ESP_LOGI(TAG, "I2C bus initialized");
    } else {
        ESP_LOGE(TAG, "Failed to init I2C bus: %s", esp_err_to_name(ret));
    }
    return ret;
}

i2c_master_bus_handle_t i2c_bus_get_handle(void) {
    return s_i2c_bus;
}