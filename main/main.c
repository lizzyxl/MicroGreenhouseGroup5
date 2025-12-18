#include <stdio.h>
#include "light_sensor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define TAG "LDR"
#define DAY_THRESHOLD 1500  // Example threshold for day
#define NIGHT_THRESHOLD 1000 // Example threshold for night

void app_main(void)
{
    // Initialize the LDR sensor before using
    ldr_init();

    int is_day = 1; // Assume it's day initially

    // Keep reading sensor values
    while (1)
    {
        int raw = ldr_read_raw();           // get raw ADC value
        float voltage = ldr_read_voltage(); // convert to voltage

        // If reading was successful, log the values
        if (raw >= 0) {
            if (is_day && raw < NIGHT_THRESHOLD) {
                is_day = 0;
                ESP_LOGI(TAG, "switch to NIGHT mode");
            } else if (!is_day && raw > DAY_THRESHOLD) {
                is_day = 1;
                ESP_LOGI(TAG, "switch to DAY mode");
            }
            ESP_LOGI(TAG, "LDR Raw: %d, Voltage: %.2f V | State: %s", raw, voltage, is_day ? "DAY" : "NIGHT");
        }
        else {
            ESP_LOGE(TAG, "Error reading LDR values");
        }

        // Delay for a second before next reading
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}