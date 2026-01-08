#include <stdio.h>
#include "esp_log.h"
#include "fan.h"
#include "pump.h"
#include "grow_light.h"
#include "utils.h"
#include "display.h"
#include "temp_hum_sensor.h"

#define TAG "AUTOMATED_GREENHOUSE"
#define MEASUREMENT_INTERVAL_MS 1000
#define DISPLAY_INTERVAL_MS 100

static uint32_t last_measurement_time = 0;
static uint32_t last_display_time = 0;

void app_main(void)
{   
    // Initialize components
    fan_init();
    pump_init();
    grow_light_init();

    ESP_LOGI(TAG, "Initializing components completed");

    float temperature = 0;
    float soil_moisture = 0;
    float relative_humidity = 0;
    int light = 0;

    int button_press;

    measurements_t current_measurements = {
        .temperature = 0,
        .soil_moisture = 0,
        .relative_humidity = 0,
        .light = 0,
    };
    led_state current_moisture_led_state;

    while(1) {
        uint32_t now = esp_log_timestamp();
       
        if (now - last_measurement_time >= MEASUREMENT_INTERVAL_MS) {
            // sensor measurements
            soil_moisture = 0;
            light = 0;
            dht11_read(&temperature, &relative_humidity);

            current_measurements.temperature = temperature;
            current_measurements.soil_moisture = soil_moisture;
            current_measurements.relative_humidity = relative_humidity;
            current_measurements.light = light;
            
            // Cloud update (TODO)

            // actuators
            fan_control(temperature);
            current_moisture_led_state = pump_control(soil_moisture);
            grow_light_control(light);
            moisture_led_control(current_moisture_led_state);

            last_measurement_time = now;
        }

        // outputs
        if (now - last_display_time >= DISPLAY_INTERVAL_MS) {
            // button input
            button_press = 0;
            //draw display
            display_draw(&current_measurements, button_press);
            
            last_display_time = now;
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
