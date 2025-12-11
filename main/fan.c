#include "fan.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define TAG "FAN_CONTROL"

volatile bool fan_state = false;
QueueHandle_t temp_queue = NULL;

static void fan_task(void *arg) {
    temp_status_t status;

    while (1) {
        if (xQueueReceive(temp_queue, &status, pdMS_TO_TICKS(0)) == pdTRUE) {
            if (status.temperature >= FAN_TEMP_HIGHER_THRESHOLD && !fan_state) {
                gpio_set_level(FAN_GPIO, 1);
                fan_state = true;
                ESP_LOGI(TAG, "FAN ON");
            } else if (status.temperature <= FAN_TEMP_LOWER_THRESHOLD && fan_state) {
                gpio_set_level(FAN_GPIO, 0);
                fan_state = false;
                ESP_LOGI(TAG, "FAN OFF");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(FAN_CHECK_INTERVAL_MS));
    }
}

bool fan_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << FAN_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    gpio_set_level(FAN_GPIO, 0);

    ESP_LOGI(TAG, "Fan GPIO configured");
    return true;
}

void fan_create_task(void) {
    xTaskCreate(fan_task, "fan", 4096, NULL, 2, NULL);
    ESP_LOGI(TAG, "Fan task created");
}
