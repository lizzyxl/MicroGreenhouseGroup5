#include "pump.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define TAG "PUMP_CONTROL"

static bool blink_state = false;
static volatile int64_t last_blink_time = 0;
static volatile int64_t pump_start_time = 0;
static float pump_start_moisture = 0;

QueueHandle_t moisture_queue;
bool water_empty = false;
bool pump_state = false;
static volatile led_state current_led_state = LED_OFF;

static void moisture_led_task(void *arg) {
    int64_t now;
    
    while (1) {
        now = esp_timer_get_time() / 1000;;
        
        switch (current_led_state) {
            case LED_OFF:
                gpio_set_level(MOISTURE_LED, 0);
                break;
            case LED_ON:
                gpio_set_level(MOISTURE_LED, 1);
                break;
            case LED_BLINKING:
                if (now - last_blink_time >= BLINK_INTERVAL_MS) {
                    blink_state = !blink_state;
                    gpio_set_level(MOISTURE_LED, blink_state);
                    last_blink_time = now;
                }
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void pump_task(void *arg) {
    moisture_status_t status;

    while (1) {
        if (xQueueReceive(moisture_queue, &status, pdMS_TO_TICKS(5000)) == pdTRUE) {
            if (pump_state && water_empty) {
                gpio_set_level(PUMP_GPIO, 0);
                pump_state = false;
                current_led_state = LED_BLINKING;
                ESP_LOGI(TAG, "PUMP OFF (Water empty)");
            } else if (status.moisture <= PUMP_MOIST_LOWER_THRESHOLD && !pump_state && !water_empty) {
                gpio_set_level(PUMP_GPIO, 1);
                pump_state = true;
                ESP_LOGI(TAG, "PUMP ON (Moisture below threshold)");
                pump_start_time = esp_timer_get_time() / 1000;
                pump_start_moisture = status.moisture;
                current_led_state = LED_ON;
            } else if (status.moisture >= PUMP_MOIST_HIGHER_THRESHOLD && pump_state && !water_empty) {
                gpio_set_level(PUMP_GPIO, 0);
                pump_state = false;
                ESP_LOGI(TAG, "PUMP OFF (Moisture in optimal range)");
                current_led_state = LED_OFF;
            }
            
            if (pump_state && !water_empty) {
                int64_t now_ms = esp_timer_get_time() / 1000;
                if ((now_ms - pump_start_time) > RESERVOIR_EMPTY_TIMEOUT) {
                    float moisture_change = pump_start_moisture - status.moisture;
                    if (moisture_change < RESERVOIR_EMPTY_TRESHOLD) {
                        water_empty = true;
                    }
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(PUMP_CHECK_TIME));
    }
}

bool pump_init(void) {
    gpio_config_t io_conf_pump = {
        .pin_bit_mask = (1ULL << PUMP_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf_pump);
    gpio_set_level(PUMP_GPIO, 0);
    
    gpio_config_t io_conf_led = {
        .pin_bit_mask = (1ULL << MOISTURE_LED),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf_led);
    gpio_set_level(MOISTURE_LED, 0);
    
    ESP_LOGI(TAG, "Pump and moisture LED GPIO configured");
    return true;
}

void pump_create_task(void) {
    xTaskCreate(pump_task, "pump", 4096, NULL, 2, NULL);
    xTaskCreate(moisture_led_task, "moisture_led", 4096, NULL, 2, NULL);
}
