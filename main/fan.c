#include "fan.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "config.h"

#define TAG "FAN_CONTROL"

void fan_control(float temperature, float temp_lower_treshold, float temp_higher_treshold) {
    static bool fan_state = false;

    if (temperature >= temp_higher_treshold && !fan_state) {
        gpio_set_level(FAN_GPIO, 0); // reversed due to relay
        fan_state = true;
        ESP_LOGI(TAG, "FAN ON");
    } else if (temperature <= temp_lower_treshold && fan_state) {
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