#ifndef TEMP_HUM_SENSOR_H
#define TEMP_HUM_SENSOR_H

#define SENSOR_TYPE DHT_TYPE_DHT11

void dht11_init();
void dht11_read(float *temperature, float *humidity);

#endif