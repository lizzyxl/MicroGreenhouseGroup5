#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "grow_light.h"
#include "esp_log.h"
#include "config.h"
#include "esp_log.h"
#include "parameter_config.h"

#define TAG "GROW_LIGHT"

void grow_light_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GROW_LIGHT_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    gpio_set_level(GROW_LIGHT_GPIO, 1);

    ESP_LOGI(TAG, "Grow Light configured");
}

void grow_light_control(float light_level, greenhouse_config_t greenhouse_config) {
    bool grow_light_state;
    if (greenhouse_config.growlight_override) {
        // MANUAL override mode
        grow_light_state = greenhouse_config.growlight_override_state;
        ESP_LOGI("GROWLIGHT", "Manual override: %s", grow_light_state ? "ON" : "OFF");
    } else {
        // AUTO mode
        grow_light_state = (light_level < greenhouse_config.growlight_light_threshold_pct);
    }
    gpio_set_level(GROW_LIGHT_GPIO, !grow_light_state);
}