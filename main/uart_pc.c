#include "uart_pc.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "freertos/task.h"
#include <string.h>

#define TAG "UART_PC"

static const int UART_TASK_STACK = 4096;

char incoming_buffer[BUF_SIZE];
int incoming_pos = 0;

static void uart_pc_to_esp_task(void *arg) {
    char byte;
    while (1) {
        int len = uart_read_bytes(UART_PC, &byte, 1, 20 / portTICK_PERIOD_MS);
        if (len > 0) {
            uart_write_bytes(UART_PC, &byte, 1);
            
            if (byte == '\n' || byte == '\r') {
                uart_write_bytes(UART_PC, "\n", 1);
                incoming_buffer[incoming_pos] = '\0';
                ESP_LOGI(TAG, "PC command received: '%s'", incoming_buffer);
                incoming_pos = 0;
                
                if (strcmp(incoming_buffer, "water_empty") == 0) {
                    extern bool water_empty;
                    water_empty = !water_empty;
                    if (water_empty) {
                        ESP_LOGI(TAG, "Setting water to empty");
                    } else {
                        ESP_LOGI(TAG, "Setting water to refilled");
                    }
                } else {
                    ESP_LOGI(TAG, "Unknown command");
                }
            } else if (incoming_pos < BUF_SIZE - 1) {
                incoming_buffer[incoming_pos++] = byte;
            }
        }
    }
}

void uart_pc_init(void) {
    uart_config_t config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    
    uart_param_config(UART_PC, &config);
    uart_driver_install(UART_PC, BUF_SIZE*2, 0, 0, NULL, 0);
    ESP_LOGI(TAG, "UART port initialized");
}

void uart_pc_create_task(void) {
    xTaskCreate(uart_pc_to_esp_task, "pc_to_esp", UART_TASK_STACK, NULL, 1, NULL);
}
