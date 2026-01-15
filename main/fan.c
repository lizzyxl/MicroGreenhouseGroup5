#include "fan.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "config.h"

#define TAG "FAN_CONTROL"

void fan_control(float temperature) {
    static bool fan_state = false;

    if (temperature >= FAN_TEMP_HIGHER_THRESHOLD && !fan_state) {
        gpio_set_level(FAN_GPIO, 0); // reversed due to relay
        fan_state = true;
        ESP_LOGI(TAG, "FAN ON");
    } else if (temperature <= FAN_TEMP_LOWER_THRESHOLD && fan_state) {
        gpio_set_level(FAN_GPIO, 1); // reversed due to relay
        fan_state = false;
        ESP_LOGI(TAG, "FAN OFF");
    }
}

void fan_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << FAN_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    gpio_set_level(FAN_GPIO, 1);

    ESP_LOGI(TAG, "Fan GPIO configured");
}