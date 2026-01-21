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
#include "wifi.h"
#include "mqtt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "adc.h"

#define TAG "AUTOMATED_GREENHOUSE"
#define DISPLAY_INTERVAL_MS 100

// WiFi config
#define WIFI_SSID "Lackner@mobile"
#define WIFI_PASS "yvka1961"

// Config
#define DEFAULT_MEASUREMENT_INTERVAL_MS 5000
#define DEFAULT_FAN_TEMP_HIGHER_THRESHOLD 25.0f
#define DEFAULT_FAN_TEMP_LOWER_THRESHOLD 20.0f
#define DEFAULT_PUMP_SOILMOIST_LOWER_THRESHOLD 30.0f
#define DEFAULT_PUMP_SOILMOIST_HIGHER_THRESHOLD 50.0f
#define DEFAULT_GROWLIGHT_LIGHT_THRESHOLD 50.0f

typedef struct
{
    uint32_t measurement_interval_ms;
    float fan_temp_higher_threshold_C;
    float fan_temp_lower_threshold_C;
    float pump_soilmoist_lower_threshold_pct;
    float growlight_light_threshold_pct;
} greenhouse_config_t;

const greenhouse_config_t default_config = {
    .measurement_interval_ms = DEFAULT_MEASUREMENT_INTERVAL_MS,
    .fan_temp_lower_threshold_C = DEFAULT_FAN_TEMP_LOWER_THRESHOLD,
    .fan_temp_higher_threshold_C = DEFAULT_FAN_TEMP_HIGHER_THRESHOLD,
    .pump_soilmoist_lower_threshold_pct = DEFAULT_PUMP_SOILMOIST_LOWER_THRESHOLD,
    .growlight_light_threshold_pct = DEFAULT_GROWLIGHT_LIGHT_THRESHOLD,
};

static greenhouse_config_t greenhouse_config = default_config;

static uint32_t last_measurement_time = 0;
static uint32_t last_display_time = 0;

void take_measurement(measurements_t *measurment)
{
    measurment->soil_moisture = soil_sensor_read();
    measurment->light = ldr_read_percent();
    //aht20_read(&measurment->temperature, &measurment->relative_humidity);

    ESP_LOGI(TAG, "Measurement taken: temperature: %.2f C, relative humidity: %.2f %%, soil moisture: %.2f %%, light intensity: %.2f %%", measurment->temperature, measurment->relative_humidity, measurment->soil_moisture, measurment->light);
}

void reset_to_default_config(greenhouse_config_t *config)
{
    memcpy(config, &default_config, sizeof(greenhouse_config_t));
}

void app_main(void)
{   
    //  Initialize sensors
    adc_init_all();
    ldr_init();
    soil_sensor_init();
    //aht20_init();
    // Initialize acctuators
    fan_init();
    pump_init();
    grow_light_init();
    // Initialize user Interface
    greenhouse_display_init();
    inputs_init();
    outputs_init();

    // Initialize Non volatile storage (to store credentials etc)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    // WiFi init
    esp_err_t ret1 = wifi_init_sta(WIFI_SSID, WIFI_PASS);
    if (ret1 == ESP_OK) {
        ESP_LOGI(TAG, "WiFi connected successfully");
    } else {
        ESP_LOGE(TAG, "WiFi connection FAILED (error: %s)", esp_err_to_name(ret));
        set_red_connection_led(LED_ON);
    }

    // MQTT init
    ESP_LOGI(TAG, "Starting MQTT...");
    ESP_ERROR_CHECK(mqtt_start());
    ESP_LOGI(TAG, "Initializing of all components completed");

    measurements_t current_measurements = {
        .temperature = 0,
        .soil_moisture = 0,
        .relative_humidity = 0,
        .light = 0,
    };

    while (1)
    {
        uint32_t now = esp_log_timestamp();

        if (now - last_measurement_time >= greenhouse_config.measurement_interval_ms)
        {

            // sensor measurements
            take_measurement(&current_measurements);

            // Publish measurements
            if(!wifi_is_connected()) {
                ESP_LOGI(TAG, "Trying reconect to wifi");
                ESP_ERROR_CHECK(wifi_reconnect());
                if (ret1 == ESP_OK) {
                    ESP_LOGI(TAG, "WiFi connected successfully");
                    set_red_connection_led(LED_OFF);
                } else {
                    ESP_LOGE(TAG, "WiFi connection FAILED (error: %s)", esp_err_to_name(ret));
                    set_red_connection_led(LED_ON);
                }
            }
            if (wifi_is_connected()) {
                if (mqtt_is_connected()) {
                char buf[16];

                // publish moisture percentage
                snprintf(buf, sizeof(buf), "%.2f", current_measurements.temperature);
                mqtt_publish("greenhouse/temperature", buf, 1, 0);

                // Relative humidity
                snprintf(buf, sizeof(buf), "%.2f", current_measurements.relative_humidity);
                mqtt_publish("greenhouse/humidity", buf, 1, 0);

                // Soil moisture
                snprintf(buf, sizeof(buf), "%.2f", current_measurements.soil_moisture);
                mqtt_publish("greenhouse/soil", buf, 1, 0);

                // Light intensity
                snprintf(buf, sizeof(buf), "%.2f", current_measurements.light);
                mqtt_publish("greenhouse/light", buf, 1, 0);
                } else {
                    ESP_LOGW(TAG, "MQTT not connected, skipping publish");
                }
            }
            
            // actuators
            fan_control(current_measurements.temperature, greenhouse_config.fan_temp_lower_threshold_C, greenhouse_config.fan_temp_higher_threshold_C);
            pump_control(current_measurements.soil_moisture, greenhouse_config.pump_soilmoist_lower_threshold_pct);
            grow_light_control(current_measurements.light, greenhouse_config.growlight_light_threshold_pct);

            last_measurement_time = now;
        }

        // outputs
        if (now - last_display_time >= DISPLAY_INTERVAL_MS)
        {
            if (get_blue_button_pressed())
            {
                ESP_LOGI(TAG, "Taking extra measurement");
                take_measurement(&current_measurements);
            }

            // leds
            green_moisture_led_control();
            red_connection_led_control();

            //draw display
            display_draw(&current_measurements, get_white_button_pressed());
            
            last_display_time = now;
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
