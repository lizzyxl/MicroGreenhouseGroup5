#include <stdio.h>
#include "light_sensor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    ldr_init();

    while(1) {
        int raw = ldr_read_raw();
        float voltage = ldr_read_voltage();

        if (raw >=0) {
            printf("LDR Raw: %d, Voltage: %.2f V\n", raw, voltage);
        } else {
            printf("Error reading LDR values\n");
        }
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
}