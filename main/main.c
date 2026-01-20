#include <stdio.h>
#include "esp_log.h"
#include "fan.h"
#include "pump.h"
#include "grow_light.h"
#include "utils.h"
#include "display.h"
#include "temp_hum_sensor.h"
#include "light_sensor.h"
#include "inputs.h"
#include "soil_moisture.h"
#include "outputs.h"
#include "config.h"

#define TAG "AUTOMATED_GREENHOUSE"
#define DISPLAY_INTERVAL_MS 100

// Config
#define DEFAULT_MEASUREMENT_INTERVAL_MS 5000
#define DEFAULT_FAN_TEMP_HIGHER_THRESHOLD 25.0f
#define DEFAULT_FAN_TEMP_LOWER_THRESHOLD 20.0f
#define DEFAULT_PUMP_SOILMOIST_LOWER_THRESHOLD 30.0f
#define DEFAULT_PUMP_SOILMOIST_HIGHER_THRESHOLD 50.0f
#define DEFAULT_GROWLIGHT_LIGHT_THRESHOLD 50.0f

typedef struct {
    uint32_t measurement_interval_ms;
    float fan_temp_higher_threshold_C;
    float fan_temp_lower_threshold_C;
    float pump_soilmoist_lower_threshold_pct;
    float pump_soilmoist_higher_threshold_pct;
    float growlight_light_threshold_pct;
} greenhouse_config_t;

const greenhouse_config_t default_config = {
    .measurement_interval_ms = DEFAULT_MEASUREMENT_INTERVAL_MS,
    .fan_temp_lower_threshold_C = DEFAULT_FAN_TEMP_LOWER_THRESHOLD,
    .fan_temp_higher_threshold_C = DEFAULT_FAN_TEMP_HIGHER_THRESHOLD,
    .pump_soilmoist_lower_threshold_pct = DEFAULT_PUMP_SOILMOIST_LOWER_THRESHOLD,
    .pump_soilmoist_higher_threshold_pct = DEFAULT_PUMP_SOILMOIST_HIGHER_THRESHOLD,
    .growlight_light_threshold_pct = DEFAULT_GROWLIGHT_LIGHT_THRESHOLD,
};

static greenhouse_config_t greenhouse_config = default_config;

static uint32_t last_measurement_time = 0;
static uint32_t last_display_time = 0;

void take_measurement (measurements_t *measurment) {
    measurment->soil_moisture = soil_sensor_read();
    measurment->light = ldr_read_percent();
    aht20_read(&measurment->temperature, &measurment->relative_humidity);

    ESP_LOGI(TAG, "Measurement taken: temperature: %.2f C, relative humidity: %.2f %%, soil moisture: %.2f %%, light intensity: %.2f %%", measurment->temperature, measurment->relative_humidity, measurment->soil_moisture, measurment->light);
            
}

void reset_to_default_config(greenhouse_config_t *config) {
    memcpy(config, &default_config, sizeof(greenhouse_config_t));
}


void app_main(void)
{   
    //greenhouse_display_init();
    // Initialize components
    fan_init();
    pump_init();
    ldr_init();
    grow_light_init();
    inputs_init();
    outputs_init();
    soil_sensor_init();
    aht20_init();

    ESP_LOGI(TAG, "Initializing of all components completed");

    measurements_t current_measurements = {
        .temperature = 0,
        .soil_moisture = 0,
        .relative_humidity = 0,
        .light = 0,
    };
    led_state current_moisture_led_state = LED_OFF;

    while(1) {
        uint32_t now = esp_log_timestamp();
       
        if (now - last_measurement_time >= greenhouse_config.measurement_interval_ms) {

            // sensor measurements
            take_measurement(&current_measurements);
            
            // Publish measurements (TODO)

            // actuators
            fan_control(current_measurements.temperature, greenhouse_config.fan_temp_lower_threshold_C, greenhouse_config.fan_temp_higher_threshold_C);
            current_moisture_led_state = pump_control(current_measurements.soil_moisture, greenhouse_config.pump_soilmoist_lower_threshold_pct, greenhouse_config.pump_soilmoist_higher_threshold_pct);
            grow_light_control(current_measurements.light, greenhouse_config.growlight_light_threshold_pct);

            last_measurement_time = now;
        }

        // outputs
        if (now - last_display_time >= DISPLAY_INTERVAL_MS) {
            // button input
            if (white_button_pressed) {
                ESP_LOGI(TAG, "Switching display");
                white_button_pressed = false;
            }

            if (blue_button_pressed) {
                ESP_LOGI(TAG, "Taking extra measurement");
                take_measurement(&current_measurements);
                blue_button_pressed = false;
            }

            // leds
            green_moisture_led_control(current_moisture_led_state);
            red_connection_led_control(current_moisture_led_state);

            //draw display
            //display_draw(&current_measurements, button_press);
            
            last_display_time = now;
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

