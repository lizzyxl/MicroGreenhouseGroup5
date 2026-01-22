#include "inputs.h"
#include "esp_timer.h"
#include "config.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "driver/uart.h"
#include "parameter_config.h"

// Button configuration
#define DEBOUNCE_TIME_MS    100

static volatile bool blue_button_pressed = false;
static volatile int blue_last_interrupt_time_ms = 0;

static volatile bool white_button_pressed = false;
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

bool get_blue_button_pressed() {
    if (!blue_button_pressed) return false;
    blue_button_pressed = false;
    return true;
}

bool get_white_button_pressed() {
    if (!white_button_pressed) return false;
    white_button_pressed = false;
    return true;
}

void uart_config_task(void *arg) {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, UART_PIN_NO_CHANGE, 17, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, 256, 0, 20, NULL, 0));
    
    uint8_t data[512];
    int json_start = -1, json_len = 0;
    
    while (1) {
        int len = uart_read_bytes(UART_NUM_1, data, sizeof(data)-1, 100 / portTICK_PERIOD_MS);
        if (len > 0) {
            data[len] = 0;  // Null terminate
            
            // Find JSON object boundaries
            for (int i = 0; i < len; i++) {
                if (data[i] == '{') {
                    json_start = i;
                    break;
                }
            }
            
            // Find end of JSON
            if (json_start >= 0) {
                for (int i = json_start; i < len; i++) {
                    if (data[i] == '}') {
                        json_len = i - json_start + 1;
                        parse_json_to_config((char*)data + json_start, json_len);
                        json_start = -1;
                        break;
                    }
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}