#include "soil_moisture.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define TAG "MAIN"

void app_main(void)
{
    soil_sensor_init();

    while (1) {
        soil_data_t soil = soil_sensor_read();

        ESP_LOGI(TAG, "Raw: %d | Voltage: %d mV | Moisture: %d%%",
                 soil.raw, soil.voltage_mv, soil.moisture_percent);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
