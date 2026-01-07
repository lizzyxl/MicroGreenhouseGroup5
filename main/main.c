#include <stdio.h>
#include "esp_log.h"
#include "fan.h"
#include "pump.h"
#include "grow_light.h"

#define TAG "AUTOMATED_GREENHOUSE"
#define MEASUREMENT_INTERVAL_MS 1000

void app_main(void)
{   
    // Initialize components
    fan_init();
    pump_init();
    grow_light_init();

    ESP_LOGI(TAG, "Initializing components completed");

    while(1) {
        // sensor measurements
        float temperature = 0;
        float moisture = 0;

        // TEMP: simulated light
        int light = get_simulated_light_level();

        // actuators
        fan_control(temperature);
        led_state current_moisture_led_state = pump_control(moisture);
        grow_light_control(light);

        // outputs
        moisture_led_control(current_moisture_led_state);

        // Cloud update (TODO)

        vTaskDelay(pdMS_TO_TICKS(MEASUREMENT_INTERVAL_MS));
    }
}
