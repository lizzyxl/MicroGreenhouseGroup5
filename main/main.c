#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "soil_moisture.h"
#include "wifi.h"
#include "mqtt.h"

#define TAG "MAIN"

// WiFi credentials
#define WIFI_SSID "TP-Link_00EF"
#define WIFI_PASS "xxxxx"

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    ESP_LOGI(TAG, "Initializing soil moisture sensor...");
    soil_sensor_init();

    ESP_LOGI(TAG, "Connecting to WiFi...");
    ESP_ERROR_CHECK(wifi_init_sta(WIFI_SSID, WIFI_PASS));
    ESP_LOGI(TAG, "WiFi connected");

    ESP_LOGI(TAG, "Starting MQTT...");
    ESP_ERROR_CHECK(mqtt_start());

    while (1)
    {
        soil_data_t soil = soil_sensor_read();

        // Local logging (unchanged behavior)
        ESP_LOGI(TAG,
                 "Raw: %d | Voltage: %d mV | Moisture: %d%%",
                 soil.raw,
                 soil.voltage_mv,
                 soil.moisture_percent);

        // Publish to MQTT (numeric payload → easy graphs)
        if (mqtt_is_connected())
        {
            char buf[16];

            // publish moisture percentage
            snprintf(buf, sizeof(buf), "%d", soil.moisture_percent);
            mqtt_publish("greenhouse/soil", buf, 1, 0);

        }
        else
        {
            ESP_LOGW(TAG, "MQTT not connected, skipping publish");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
