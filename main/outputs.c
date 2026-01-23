#include "outputs.h"
#include "config.h"
#include "esp_timer.h"

void outputs_init(void)
{
    gpio_config_t io_conf_led_red = {
        .pin_bit_mask = (1ULL << RED_WIFI_LED),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf_led_red);
    gpio_set_level(RED_WIFI_LED, 0);

    gpio_config_t io_conf_led_green = {
        .pin_bit_mask = (1ULL << MOISTURE_LED),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf_led_green);
    gpio_set_level(MOISTURE_LED, 0);
}

void set_red_connection_led(led_state state) {
    if(state == LED_ON) {
        gpio_set_level(RED_WIFI_LED, 1);
    } else {
        gpio_set_level(RED_WIFI_LED, 0);
    }
}

void set_green_moisture_led(led_state state) {
    if(state == LED_ON) {
        gpio_set_level(MOISTURE_LED, 1);
    } else {
        gpio_set_level(MOISTURE_LED, 0);
    }
}