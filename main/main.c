#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <driver/gpio.h>
#include "esp_log.h"

#include "pump.h"
#include "uart_pc.h"

extern QueueHandle_t moisture_queue;

#define MEASUREMENT_INTERVAL_MS 1000
#define TAG "MAIN"

static void simulated_moist_sensor_task(void *arg) {
    float simulated_moist = 40;
    
    while (1) {
        if (pump_state) {
            simulated_moist += -1.0f + (rand() % 6);
        } else {
            simulated_moist += -4.0f + (rand() % 6);
        }
        
        if (simulated_moist > 100.0f) simulated_moist = 100.0f;
        if (simulated_moist < 20.0f) simulated_moist = 20.0f;

        moisture_status_t status = {
            .moisture = simulated_moist
        };

        if (xQueueSend(moisture_queue, &status, pdMS_TO_TICKS(100)) == pdTRUE) {
            ESP_LOGI(TAG, "Sensor: %.1f %% (pump=%s, water_empty=%s)", 
                    simulated_moist,
                    pump_state ? "ON" : "OFF",
                    water_empty ? "YES" : "NO");
        }
        
        vTaskDelay(pdMS_TO_TICKS(MEASUREMENT_INTERVAL_MS));
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Starting Greenhouse Pump Demo");
    
    moisture_queue = xQueueCreate(10, sizeof(moisture_status_t));
    if (moisture_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create queue!");
        return;
    }
    
    uart_pc_init();
    pump_init();
    
    uart_pc_create_task();
    xTaskCreate(simulated_moist_sensor_task, "simulated_moist_sensor", 4096, NULL, 1, NULL);
    pump_create_task();
    
    ESP_LOGI(TAG, "All tasks started successfully!");
}