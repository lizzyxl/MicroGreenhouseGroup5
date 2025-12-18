#include "temp_hum_sensor.h"

void app_main(void)
{

    float temperature = 0.0;
    float humidity = 0.0;

    dht11_init();

    while (1)
    {
        dht11_read(&temperature, &humidity);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}