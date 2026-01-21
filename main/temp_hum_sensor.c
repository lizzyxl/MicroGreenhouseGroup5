#include "temp_hum_sensor.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include "aht20.h"
#include "esp_system.h"
#include "esp_log.h"
#include "i2c_bus.h" 
#include "driver/gpio.h" 
#include "config.h"

static i2c_bus_handle_t i2c_bus;
static aht20_dev_handle_t aht20 = NULL;

void aht20_init()
{
    
    const i2c_config_t i2c_bus_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ};
    i2c_bus = i2c_bus_create(I2C_MASTER_NUM, &i2c_bus_conf);

    aht20_i2c_config_t i2c_conf = {
        .bus_inst = i2c_bus,
        .i2c_addr = AHT20_ADDRRES_0,
    };

    aht20_new_sensor(&i2c_conf, &aht20);

}

void aht20_read(float *temperature, float *humidity)
{
    uint32_t temperature_raw;
    uint32_t humidity_raw;

    aht20_read_temperature_humidity(aht20, &temperature_raw, temperature, &humidity_raw, humidity);
}
