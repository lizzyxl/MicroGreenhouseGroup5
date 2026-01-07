#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "grow_light.h"

#define IR_LED_GPIO      GPIO_NUM_25
#define LIGHT_THRESHOLD  1500

static int get_simulated_light_level(void) {
    static int light = 2000;
    light -= 100;
    if (light < 500) light = 2000;
    return light;
}

void grow_light_init(void) {
    gpio_reset_pin(IR_LED_GPIO);
    gpio_set_direction(IR_LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(IR_LED_GPIO, 0);
    printf("[GROW LIGHT] Initialized on GPIO %d\n", IR_LED_GPIO);
}

void grow_light_control(int light_level) {
    if (light_level < LIGHT_THRESHOLD) {
        gpio_set_level(IR_LED_GPIO, 1);
        printf("[GROW LIGHT] ON  (light level: %d)\n", light_level);
    } else {
        gpio_set_level(IR_LED_GPIO, 0);
        printf("[GROW LIGHT] OFF (light level: %d)\n", light_level);
    }
}

void grow_light_task(void *pvParameters) {
    while (1) {
        int current_light = get_simulated_light_level();
        grow_light_control(current_light);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
