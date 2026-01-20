#include "inputs.h"
#include "esp_timer.h"
#include "config.h"
#include "esp_attr.h"

#define DEBOUNCE_TIME_MS 50

// WHITE button variables
static volatile bool white_isr_flag = false;
static int white_last_state = 1;
static int white_stable_state = 1;
static int64_t white_last_change = 0;
static bool white_press_event = false;

// BLUE button variables
static volatile bool blue_isr_flag = false;
static int blue_last_state = 1;
static int blue_stable_state = 1;
static int64_t blue_last_change = 0;
static bool blue_press_event = false;

// ISR handlers
static void IRAM_ATTR white_isr(void *arg) {
    white_isr_flag = true;
}

static void IRAM_ATTR blue_isr(void *arg) {
    blue_isr_flag = true;
}

// Update functions
static void update_white_button(void)
{
    if (!white_isr_flag)
        return;

    white_isr_flag = false;

    int64_t now = esp_timer_get_time() / 1000;

    if (now - white_last_change < DEBOUNCE_TIME_MS)
        return;

    white_last_change = now;

    int raw = gpio_get_level(BUTTON_PIN_WHITE);

    if (raw != white_last_state) {
        white_last_state = raw;

        if (raw != white_stable_state) {
            white_stable_state = raw;

            if (white_stable_state == 0) {
                white_press_event = true;
            }
        }
    }
}

static void update_blue_button(void)
{
    if (!blue_isr_flag)
        return;

    blue_isr_flag = false;

    int64_t now = esp_timer_get_time() / 1000;

    if (now - blue_last_change < DEBOUNCE_TIME_MS)
        return;

    blue_last_change = now;

    int raw = gpio_get_level(BUTTON_PIN_BLUE);

    if (raw != blue_last_state) {
        blue_last_state = raw;

        if (raw != blue_stable_state) {
            blue_stable_state = raw;

            if (blue_stable_state == 0) {
                blue_press_event = true;
            }
        }
    }
}

// init
void inputs_init(void)
{
    gpio_config_t cfg_white = {
        .pin_bit_mask = (1ULL << BUTTON_PIN_WHITE),
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };
    gpio_config(&cfg_white);

    gpio_config_t cfg_blue = {
        .pin_bit_mask = (1ULL << BUTTON_PIN_BLUE),
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };
    gpio_config(&cfg_blue);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_PIN_WHITE, white_isr, NULL);
    gpio_isr_handler_add(BUTTON_PIN_BLUE, blue_isr, NULL);
}

bool input_button_pressed_white(void)
{
    update_white_button();

    if (white_press_event) {
        white_press_event = false;
        return true;
    }
    return false;
}

bool input_button_pressed_blue(void)
{
    update_blue_button();

    if (blue_press_event) {
        blue_press_event = false;
        return true;
    }
    return false;
}
