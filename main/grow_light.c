#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "grow_light.h"
#include "esp_log.h"

#define IR_LED_GPIO      GPIO_NUM_25
#define LIGHT_THRESHOLD  1500

#define TAG "GROW_LIGHT"

void grow_light_init(void) {
    gpio_reset_pin(IR_LED_GPIO);
    gpio_set_direction(IR_LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(IR_LED_GPIO, 0);
    ESP_LOGI(TAG, "Initialized on GPIO %d\n", IR_LED_GPIO);
}

void grow_light_control(int light_level) {
    if (light_level < LIGHT_THRESHOLD) {
        gpio_set_level(IR_LED_GPIO, 1);
    } else {
        gpio_set_level(IR_LED_GPIO, 0);
    }
}

void grow_light_task(void *pvParameters) {
    while (1) {
        int current_light = 0;
        grow_light_control(current_light);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
