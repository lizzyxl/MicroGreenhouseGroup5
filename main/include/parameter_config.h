#ifndef PARAMETER_CONFIG_H
#define PARAMETER_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t measurement_interval_ms;
    float fan_temp_higher_threshold_C;
    float fan_temp_lower_threshold_C;
    float pump_soilmoist_lower_threshold_pct;
    float growlight_light_threshold_pct;
    bool growlight_override;      
    bool growlight_override_state; 
    bool pump_override;
    bool pump_override_state;
    bool fan_override;
    bool fan_override_state;
} greenhouse_config_t;

extern greenhouse_config_t greenhouse_config;
void reset_to_default_config(void);

#endif
