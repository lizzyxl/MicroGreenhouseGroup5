#include "temp_hum_sensor.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "dht.h"

void dht11_read(float *temperature, float *humidity)
{
    // Read humidity and temperature from the sensor
    if (dht_read_float_data(SENSOR_TYPE, DHT_GPIO, humidity, temperature) == ESP_OK)
    {
        //printf("Temperature: %.1f °C, Humidity: %.1f %%\n", *temperature, *humidity);
    }
    else
    {
        printf("Could not read data from sensor\n");
    }

}
