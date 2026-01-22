#ifndef I2C_INIT_H
#define I2C_INIT_H

#include <driver/i2c_master.h>

esp_err_t i2c_bus_init(void);
i2c_master_bus_handle_t i2c_bus_get_handle(void);

#endif