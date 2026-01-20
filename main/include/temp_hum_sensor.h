#define I2C_MASTER_FREQ_HZ 100000     /*!< I2C master clock frequency */

void aht20_init();
void aht20_read(float *temperature, float *humidity);
