#include "fan.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "config.h"
#include "parameter_config.h"

#define TAG "FAN_CONTROL"

void fan_control(float temperature, float humidity, greenhouse_config_t greenhouse_config) {
    static bool fan_state;
     if (greenhouse_config.fan_override) {
        // MANUAL override mode
        fan_state = greenhouse_config.fan_override_state;
        ESP_LOGI(TAG, "Manual override: %s", fan_state ? "ON" : "OFF");
    } else {
        // AUTO mode
        if ((temperature >= greenhouse_config.fan_temp_higher_threshold_C || humidity >= greenhouse_config.fan_hum_higher_threshold_pct) && !fan_state) {
            fan_state = true;
            ESP_LOGI(TAG, "FAN ON");
        } else if (temperature <= greenhouse_config.fan_temp_lower_threshold_C &&  humidity <= greenhouse_config.fan_hum_lower_threshold_pct && fan_state) {
            fan_state = false;
            ESP_LOGI(TAG, "FAN OFF");
        }
    }
    gpio_set_level(FAN_GPIO, !fan_state); //inverse due to relay
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