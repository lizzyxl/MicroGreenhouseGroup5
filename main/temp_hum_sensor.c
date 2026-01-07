#include "temp_hum_sensor.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "dht.h"

void dht11_init()
{
    // Enable pull-up resistor on the DHT data pin
    //gpio_set_pull_mode(DHT_GPIO, GPIO_PULLUP_ONLY);
}

void dht11_read(float *temperature, float *humidity)
{
    // Read humidity and temperature from the sensor
    if (dht_read_float_data(SENSOR_TYPE, DHT_GPIO, humidity, temperature) == ESP_OK)
    {
        // Print the sensor values if read was successful
        //printf("Humidity: %.1f%%  Temp: %.1f°C\n", *humidity, *temperature);
    }
    else
    {
        printf("Could not read data from sensor\n");
    }

}
