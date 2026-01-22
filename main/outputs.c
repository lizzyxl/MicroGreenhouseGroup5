#include "outputs.h"
#include "config.h"
#include "esp_timer.h"

#define BLINK_INTERVAL_MS 1000 //blink intervall of led

static led_state red_led_target_state = LED_OFF;
static led_state green_led_target_state = LED_OFF;

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
    red_led_target_state = state;
    red_connection_led_control();
}

void set_green_moisture_led(led_state state) {
    green_led_target_state = state;
    green_moisture_led_control();
}

led_state get_green_moisture_led(void) {
    return green_led_target_state;
}

void red_connection_led_control() {
    static int64_t last_blink_time = 0; 
    int64_t now;
    static bool blink_state = false;
    
    switch (red_led_target_state) {
        case LED_OFF:
            gpio_set_level(RED_WIFI_LED, 0);
            break;
        case LED_ON:
            gpio_set_level(RED_WIFI_LED, 1);
            break;
        case LED_BLINKING:
            now = esp_timer_get_time() / 1000;
            if (now - last_blink_time >= BLINK_INTERVAL_MS) {
                blink_state = !blink_state;
                gpio_set_level(RED_WIFI_LED, blink_state);
                last_blink_time = now;
            }
            break;
    }
}

void green_moisture_led_control() {
    static int64_t last_blink_time = 0; 
    int64_t now;
    static bool blink_state = false;
    
    switch (green_led_target_state) {
        case LED_OFF:
            gpio_set_level(MOISTURE_LED, 0);
            break;
        case LED_ON:
            gpio_set_level(MOISTURE_LED, 1);
            break;
        case LED_BLINKING:
            now = esp_timer_get_time() / 1000;
            if (now - last_blink_time >= BLINK_INTERVAL_MS) {
                blink_state = !blink_state;
                gpio_set_level(MOISTURE_LED, blink_state);
                last_blink_time = now;
            }
            break;
    }
}