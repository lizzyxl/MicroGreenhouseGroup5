#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "fan.h"
#include "esp_log.h"

#define TEMP_INTERVAL_MS 1000

extern QueueHandle_t temp_queue;
extern volatile bool fan_state;

static void simulated_temp_sensor_task(void *arg) {
    static float simulated_temp = 20;
    float temp_change;
    
    while (1) {
        if (fan_state) {
            temp_change = -1.5f + (rand() % 11) * 0.2f;
        } else {
            temp_change = -0.5f + (rand() % 11) * 0.2f;
        }
        
        simulated_temp += temp_change;
        if (simulated_temp > 35.0f) simulated_temp = 35.0f;
        if (simulated_temp < 10.0f) simulated_temp = 10.0f;
        
        temp_status_t status = {
            .temperature = simulated_temp
        };
        xQueueSend(temp_queue, &status, 0);
          
        ESP_LOGI("MAIN", "Sensor: %.1f°C (fan=%s)", simulated_temp, 
                fan_state ? "ON" : "OFF");
        
        vTaskDelay(pdMS_TO_TICKS(TEMP_INTERVAL_MS));
    }
}

void app_main(void) {
    ESP_LOGI("MAIN", "Starting Fan Demo");
    
    temp_queue = xQueueCreate(10, sizeof(temp_status_t));
    
    fan_init();
    
    xTaskCreate(simulated_temp_sensor_task, "temp_sensor", 4096, NULL, 1, NULL);
    fan_create_task();
    
    ESP_LOGI("MAIN", "All tasks started");
}