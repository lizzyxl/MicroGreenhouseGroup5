#define DHT_GPIO 27
#define SENSOR_TYPE DHT_TYPE_DHT11

void dht11_init();

void dht11_read(float *temperature, float *humidity);