#include "inputs.h"
#include "esp_timer.h"
#include "config.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

// Button configuration
#define DEBOUNCE_TIME_MS    100

volatile bool blue_button_pressed = false;
static volatile int blue_last_interrupt_time_ms = 0;

volatile bool white_button_pressed = false;
static volatile int white_last_interrupt_time_ms = 0;

void IRAM_ATTR blue_button_isr_handler(void *arg) {
    int now_ms = esp_timer_get_time() / 1000; // function returns time in microseconds

    // debounce
    if ((now_ms - blue_last_interrupt_time_ms) > DEBOUNCE_TIME_MS)
    {
        blue_button_pressed = true;
        blue_last_interrupt_time_ms = now_ms;
    }
}

void IRAM_ATTR white_button_isr_handler(void *arg) {
    int now_ms = esp_timer_get_time() / 1000; // function returns time in microseconds

    // debounce
    if ((now_ms - white_last_interrupt_time_ms) > DEBOUNCE_TIME_MS)
    {
        white_button_pressed = true;
        white_last_interrupt_time_ms = now_ms;
    }
}

void inputs_init(void)
{
    gpio_config_t cfg_blue = {
        .pin_bit_mask = (1ULL << BUTTON_PIN_BLUE),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };
    gpio_config(&cfg_blue);

        gpio_config_t cfg_white = {
        .pin_bit_mask = (1ULL << BUTTON_PIN_WHITE),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };
    gpio_config(&cfg_white);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_PIN_BLUE, blue_button_isr_handler, (void *) BUTTON_PIN_BLUE);
    gpio_isr_handler_add(BUTTON_PIN_WHITE, white_button_isr_handler, (void *) BUTTON_PIN_WHITE);
}
