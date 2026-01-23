#include "parameter_config.h"
#include <stdbool.h>
#include <cJSON.h>
#include "esp_log.h"
#include <stdlib.h>

#define MAX_JSON_SIZE 512
#define TAG "CONFIG_HANDLER"

#define DEFAULT_MEASUREMENT_INTERVAL_S 10
#define DEFAULT_FAN_TEMP_HIGHER_THRESHOLD 23.0f
#define DEFAULT_FAN_TEMP_LOWER_THRESHOLD 22.0f
#define DEFAULT_FAN_HUM_HIGHER_THRESHOLD 70.0f
#define DEFAULT_FAN_HUM_LOWER_THRESHOLD 65.0f
#define DEFAULT_PUMP_SOILMOIST_THRESHOLD 25.0f
#define DEFAULT_GROWLIGHT_LIGHT_THRESHOLD 50.0f
#define DEFAULT_WIFI_SSID "Lackner@mobile"
#define DEFAULT_WIFI_PASS "yvka1961"

greenhouse_config_t greenhouse_config;

void reset_to_default_config(void) {
    greenhouse_config.measurement_interval_s = DEFAULT_MEASUREMENT_INTERVAL_S;
    greenhouse_config.fan_temp_lower_threshold_C = DEFAULT_FAN_TEMP_LOWER_THRESHOLD;
    greenhouse_config.fan_temp_higher_threshold_C = DEFAULT_FAN_TEMP_HIGHER_THRESHOLD;
    greenhouse_config.fan_hum_lower_threshold_pct = DEFAULT_FAN_HUM_LOWER_THRESHOLD;
    greenhouse_config.fan_hum_higher_threshold_pct = DEFAULT_FAN_HUM_HIGHER_THRESHOLD;
    greenhouse_config.pump_soilmoist_threshold_pct = DEFAULT_PUMP_SOILMOIST_THRESHOLD;
    greenhouse_config.growlight_light_threshold_pct = DEFAULT_GROWLIGHT_LIGHT_THRESHOLD;
    greenhouse_config.growlight_override = false;
    greenhouse_config.growlight_override_state = false;
    greenhouse_config.pump_override = false;
    greenhouse_config.pump_override_state = false;
    greenhouse_config.fan_override = false;
    greenhouse_config.fan_override_state = false;
    greenhouse_config.wifi_reconfigure = false;
    greenhouse_config.config_updated = false;
    strcpy(greenhouse_config.wifi_ssid, DEFAULT_WIFI_SSID);
    strcpy(greenhouse_config.wifi_password, DEFAULT_WIFI_PASS);
}

void parse_json_to_config(const char *data, int len) {
    // check buffer overflow for security reason
    if (len > MAX_JSON_SIZE || len <= 0) {
        ESP_LOGE(TAG, "Invalid JSON size: %d (max %d)", len, MAX_JSON_SIZE);
        return;
    }
    
    // safe copy and null terminate
    char *json = malloc(len + 1);
    if (!json) {
        ESP_LOGE(TAG, "Malloc failed");
        return;
    }
    memcpy(json, data, len);
    json[len] = '\0';
    
    // parsing
    cJSON *root = cJSON_Parse(json);
    if (!root) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr) {
            ESP_LOGE(TAG, "JSON parse error before: %s", error_ptr);
        }
        free(json);
        return;
    }
     
    bool config_changed = false;
    
    // parse and validate fields
    cJSON *field;
    
    // Measurement interval (1s-24h)
    if ((field = cJSON_GetObjectItem(root, "measurement_interval_s"))) {
        int val = field->valueint;
        if (val >= 1 && val <= 86400) {
            greenhouse_config.measurement_interval_s = val;
            config_changed = true;
        } else {
            ESP_LOGW(TAG, "Invalid interval: %d (1-86400)", val);
        }
    }
    
    // Fan thresholds (0-50°C)
    if ((field = cJSON_GetObjectItem(root, "fan_temp_lower_threshold_C"))) {
        float val = field->valuedouble;
        if (val >= 0.0f && val <= 50.0f) {
            greenhouse_config.fan_temp_lower_threshold_C = val;
            config_changed = true;
        }
    }
    if ((field = cJSON_GetObjectItem(root, "fan_temp_higher_threshold_C"))) {
        float val = field->valuedouble;
        if (val >= 0.0f && val <= 50.0f) {
            greenhouse_config.fan_temp_higher_threshold_C = val;
            config_changed = true;
        }
    }
    if ((field = cJSON_GetObjectItem(root, "fan_hum_lower_threshold_pct"))) {
        float val = field->valuedouble;
        if (val >= 0.0f && val <= 100.0f) {
            greenhouse_config.fan_hum_lower_threshold_pct = val;
            config_changed = true;
        }
    }
    if ((field = cJSON_GetObjectItem(root, "fan_hum_higher_threshold_pct"))) {
        float val = field->valuedouble;
        if (val >= 0.0f && val <= 100.0f) {
            greenhouse_config.fan_hum_higher_threshold_pct = val;
            config_changed = true;
        }
    }
    
    // Pump threshold (0-100%)
    if ((field = cJSON_GetObjectItem(root, "pump_soilmoist_threshold_pct"))) {
        float val = field->valuedouble;
        if (val >= 0.0f && val <= 100.0f) {
            greenhouse_config.pump_soilmoist_threshold_pct = val;
            config_changed = true;
        }
    }
    
    // Growlight threshold (0-100%)
    if ((field = cJSON_GetObjectItem(root, "growlight_light_threshold_pct"))) {
        float val = field->valuedouble;
        if (val >= 0.0f && val <= 100.0f) {
            greenhouse_config.growlight_light_threshold_pct = val;
            config_changed = true;
        }
    }
    
    // Overrides (boolean)
    if ((field = cJSON_GetObjectItem(root, "fan_override"))) {
        greenhouse_config.fan_override = cJSON_IsTrue(field);
        config_changed = true;
    }
    if ((field = cJSON_GetObjectItem(root, "fan"))) {
        greenhouse_config.fan_override_state = cJSON_IsTrue(field);
        greenhouse_config.fan_override = true;
        config_changed = true;
    }
    
    if ((field = cJSON_GetObjectItem(root, "pump_override"))) {
        greenhouse_config.pump_override = cJSON_IsTrue(field);
        config_changed = true;
    }
    if ((field = cJSON_GetObjectItem(root, "pump"))) {
        greenhouse_config.pump_override_state = cJSON_IsTrue(field);
        greenhouse_config.pump_override = true;
        config_changed = true;
    }
    
    if ((field = cJSON_GetObjectItem(root, "growlight_override"))) {
        greenhouse_config.growlight_override = cJSON_IsTrue(field);
        config_changed = true;
    }
    if ((field = cJSON_GetObjectItem(root, "growlight"))) {
        greenhouse_config.growlight_override_state = cJSON_IsTrue(field);
        greenhouse_config.growlight_override = true;
        config_changed = true;
    }
    if ((field = cJSON_GetObjectItem(root, "config"))) {
        if (cJSON_IsString(field) && strcmp(field->valuestring, "default") == 0) {
            reset_to_default_config();
            config_changed = true;
        }
    }
    if ((field = cJSON_GetObjectItem(root, "wifi_ssid"))) {
        if (cJSON_IsString(field) && strlen(field->valuestring) <= 32) {
            strncpy(greenhouse_config.wifi_ssid, field->valuestring, 32);
            greenhouse_config.wifi_ssid[32] = '\0';
            greenhouse_config.wifi_reconfigure = true;
            config_changed = true;
            ESP_LOGI(TAG, "WiFi SSID updated: %s", greenhouse_config.wifi_ssid);
        }
    }
    if ((field = cJSON_GetObjectItem(root, "wifi_password"))) {
        if (cJSON_IsString(field) && strlen(field->valuestring) <= 64) {
            strncpy(greenhouse_config.wifi_password, field->valuestring, 64);
            greenhouse_config.wifi_password[64] = '\0';
            greenhouse_config.wifi_reconfigure = true;
            config_changed = true;
            ESP_LOGI(TAG, "WiFi password updated", strlen(greenhouse_config.wifi_password));
        }
    }
    
    // cleanup
    cJSON_Delete(root);
    free(json);
    
    // trigger if changed
    if (config_changed) {
        greenhouse_config.config_updated = true;
        ESP_LOGI(TAG, "=== NEW GREENHOUSE CONFIG ===");
        ESP_LOGI(TAG, "Interval: %lu s", greenhouse_config.measurement_interval_s);
        ESP_LOGI(TAG, "Fan temp thres: %.1f°C <-> %.1f°C %.1f%% <-> %.1f%% (override: %s, state: %s)", 
                greenhouse_config.fan_temp_lower_threshold_C,
                greenhouse_config.fan_temp_higher_threshold_C,
                greenhouse_config.fan_hum_lower_threshold_pct,
                greenhouse_config.fan_hum_higher_threshold_pct,
                greenhouse_config.fan_override ? "ON" : "OFF",
                greenhouse_config.fan_override_state ? "ON" : "OFF");
        ESP_LOGI(TAG, "Pump soilm. thres: %.1f%% (override: %s, state: %s)", 
                greenhouse_config.pump_soilmoist_threshold_pct,
                greenhouse_config.pump_override ? "ON" : "OFF",
                greenhouse_config.pump_override_state ? "ON" : "OFF");
        ESP_LOGI(TAG, "Growlight light thres: %.1f%% (override: %s, state: %s)", 
                greenhouse_config.growlight_light_threshold_pct,
                greenhouse_config.growlight_override ? "ON" : "OFF",
                greenhouse_config.growlight_override_state ? "ON" : "OFF");
    }
}