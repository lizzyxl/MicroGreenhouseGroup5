#ifndef UART_PC_H
#define UART_PC_H

#include "freertos/FreeRTOS.h"

#define UART_PC UART_NUM_0
#define BUF_SIZE 256

extern char incoming_buffer[BUF_SIZE];
extern int incoming_pos;

void uart_pc_init(void);
void uart_pc_create_task(void);

#endif
