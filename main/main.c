#include <stdio.h>
#include "light_sensor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "LDR";

void app_main(void)
{
    ldr_init();

    while(1) {
        int raw = ldr_read_raw();
        float voltage = ldr_read_voltage();

        if (raw >=0) {
            ESP_LOGI(TAG, "LDR Raw: %d, Voltage: %.2f V", raw, voltage);
        } else {
            ESP_LOGE(TAG, "Error reading LDR values");
        }    
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}