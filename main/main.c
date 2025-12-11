/*
 * Grow Light Control (Infrared LED)
 * Component test code for Micro Greenhouse project
 * Author: Zibha Aissa Faycal
 * 
 * This code controls an IR LED (grow light) based on simulated light levels.
 * In the final system, the simulated data will be replaced with real LDR
 * readings received via a FreeRTOS queue from the LDR sensor task.
 */

#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Hardware configuration
#define IR_LED_GPIO      GPIO_NUM_25
#define LIGHT_THRESHOLD  1500  // Threshold for turning on grow light

// Simulated light sensor reading (for testing without LDR)
static int get_simulated_light_level(void) {
    static int light = 2000;
    light -= 100;  // Simulate gradual darkness
    if (light < 500) light = 2000;  // Reset to simulate day/night cycle
    return light;
}

// Initialize GPIO for grow light
void grow_light_init(void) {
    gpio_reset_pin(IR_LED_GPIO);
    gpio_set_direction(IR_LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(IR_LED_GPIO, 0);  // Start with LED off
    printf("[GROW LIGHT] Initialized on GPIO %d\n", IR_LED_GPIO);
}

// Control logic: turn LED on/off based on light level
void grow_light_control(int light_level) {
    if (light_level < LIGHT_THRESHOLD) {
        gpio_set_level(IR_LED_GPIO, 1);
        printf("[GROW LIGHT] ON  (light level: %d)\n", light_level);
    } else {
        gpio_set_level(IR_LED_GPIO, 0);
        printf("[GROW LIGHT] OFF (light level: %d)\n", light_level);
    }
}

// Main task for grow light control
void grow_light_task(void *pvParameters) {
    while (1) {
        // TODO: Replace with xQueueReceive(ldr_queue, ...) in final integration
        int current_light = get_simulated_light_level();
        
        grow_light_control(current_light);
        
        vTaskDelay(pdMS_TO_TICKS(5000));  // Check every 5 seconds
    }
}

// Application entry point
void app_main(void) {
    grow_light_init();
    
    xTaskCreate(grow_light_task,
                "grow_light_task",
                2048,
                NULL,
                5,
                NULL);
}