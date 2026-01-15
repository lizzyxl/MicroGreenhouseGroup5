#include "inputs.h"
#include "esp_timer.h"

// Button configuration
#define BUTTON_PIN_WHITE          GPIO_NUM_13
#define BUTTON_PIN_BLUE          GPIO_NUM_21
#define DEBOUNCE_TIME_MS    50
#define RED_WIFI_LED GPIO_NUM_12

static int last_state = 1;          // pull-up = idle high
static int stable_state = 1;
static int64_t last_change_time = 0;
static bool press_event = false;

void inputs_init(void)
{
    gpio_config_t cfg_blue = {
        .pin_bit_mask = (1ULL << BUTTON_PIN_BLUE),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&cfg_blue);

        gpio_config_t cfg_white = {
        .pin_bit_mask = (1ULL << BUTTON_PIN_WHITE),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&cfg_white);

    gpio_config_t io_conf_led = {
        .pin_bit_mask = (1ULL << RED_WIFI_LED),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf_led);
    gpio_set_level(RED_WIFI_LED, 1);
}

static void update_button(void)
{
    int raw = gpio_get_level(BUTTON_PIN_WHITE);
    int64_t now = esp_timer_get_time() / 1000; // ms

    if (raw != last_state) {
        last_change_time = now;
    }

    if ((now - last_change_time) > DEBOUNCE_TIME_MS) {
        if (raw != stable_state) {
            stable_state = raw;

            // Detect falling edge (button press)
            if (stable_state == 0) {
                press_event = true;
            }
        }
    }

    last_state = raw;
}

bool input_button_pressed(void)
{
    update_button();

    if (press_event) {
        press_event = false;
        return true;
    }

    return false;
}
