#include "pump.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "config.h"
#include "outputs.h"

#define TAG "PUMP_CONTROL"

led_state pump_control(float moisture, float soilmoist_lower_treshold_pct, float soilmoist_higher_treshold_pct) {
    led_state current_moisture_led_state = LED_OFF;
    static float pump_start_moisture = 0;
    static int64_t pump_start_time = 0;
    static bool pump_state = false;

    if (moisture <= soilmoist_lower_treshold_pct && !pump_state) { //check lower treshold 
        gpio_set_level(PUMP_GPIO, 0);
        pump_state = true;
        ESP_LOGI(TAG, "PUMP ON (Moisture below threshold)");
        pump_start_time = esp_timer_get_time() / 1000;
        pump_start_moisture = moisture;
        current_moisture_led_state = LED_ON;
    } else if (moisture >= soilmoist_higher_treshold_pct && pump_state) { //check higher treshold
        gpio_set_level(PUMP_GPIO, 1);
        pump_state = false;
        ESP_LOGI(TAG, "PUMP OFF (Moisture in optimal range)");
        current_moisture_led_state = LED_OFF;
    }

    return current_moisture_led_state;
}

void pump_init(void) {
    gpio_config_t io_conf_pump = {
        .pin_bit_mask = (1ULL << PUMP_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf_pump);
    gpio_set_level(PUMP_GPIO, 0);
    
    ESP_LOGI(TAG, "Pump configured");
}