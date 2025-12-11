#include <stdio.h>
#include "light_sensor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define TAG "LDR"

void app_main(void)
{
    // Initialize the LDR sensor before using
    ldr_init();

    // Keep reading sensor values
    while (1)
    {
        int raw = ldr_read_raw();           // get raw ADC value
        float voltage = ldr_read_voltage(); // convert to voltage

        // If reading was successful, log the values
        if (raw >= 0) {
            ESP_LOGI(TAG, "LDR Raw: %d, Voltage: %.2f V", raw, voltage);
        }
        else {
            ESP_LOGE(TAG, "Error reading LDR values");
        }

        // Delay for a second before next reading
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}