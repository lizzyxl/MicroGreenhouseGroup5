#ifndef PARAMETER_CONFIG_H
#define PARAMETER_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t measurement_interval_s;
    float fan_temp_higher_threshold_C;
    float fan_temp_lower_threshold_C;
    float fan_hum_higher_threshold_pct;
    float fan_hum_lower_threshold_pct;
    float pump_soilmoist_threshold_pct;
    float growlight_light_threshold_pct;
    bool growlight_override;      
    bool growlight_override_state; 
    bool pump_override;
    bool pump_override_state;
    bool fan_override;
    bool fan_override_state;
    char wifi_ssid[33];
    char wifi_password[65];
    bool wifi_reconfigure;
    bool config_updated;
} greenhouse_config_t;

extern greenhouse_config_t greenhouse_config;
void reset_to_default_config(void);
void parse_json_to_config(const char *data, int len);

#endif
