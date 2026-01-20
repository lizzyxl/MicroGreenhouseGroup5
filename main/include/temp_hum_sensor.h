#define I2C_MASTER_FREQ_HZ 100000     /*!< I2C master clock frequency */
static const char *TAG = "aht20 test";

void aht20_init();
void aht20_read(float *temperature, float *humidity);
