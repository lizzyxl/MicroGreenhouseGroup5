#include "parameter_config.h"
#include <stdbool.h>

#define DEFAULT_MEASUREMENT_INTERVAL_MS 5000
#define DEFAULT_FAN_TEMP_HIGHER_THRESHOLD 25.0f
#define DEFAULT_FAN_TEMP_LOWER_THRESHOLD 20.0f
#define DEFAULT_PUMP_SOILMOIST_LOWER_THRESHOLD 30.0f
#define DEFAULT_PUMP_SOILMOIST_HIGHER_THRESHOLD 50.0f
#define DEFAULT_GROWLIGHT_LIGHT_THRESHOLD 40.0f

greenhouse_config_t greenhouse_config  = {
    .measurement_interval_ms = DEFAULT_MEASUREMENT_INTERVAL_MS,
    .fan_temp_lower_threshold_C = DEFAULT_FAN_TEMP_LOWER_THRESHOLD,
    .fan_temp_higher_threshold_C = DEFAULT_FAN_TEMP_HIGHER_THRESHOLD,
    .pump_soilmoist_lower_threshold_pct = DEFAULT_PUMP_SOILMOIST_LOWER_THRESHOLD,
    .growlight_light_threshold_pct = DEFAULT_GROWLIGHT_LIGHT_THRESHOLD,
    .growlight_override = false,
    .growlight_override_state = false, 
    .pump_override = false,
    .pump_override_state = false,
    .fan_override = false,
    .fan_override_state = false,
};

void reset_to_default_config(void) {
    greenhouse_config.measurement_interval_ms = DEFAULT_MEASUREMENT_INTERVAL_MS;
    greenhouse_config.fan_temp_lower_threshold_C = DEFAULT_FAN_TEMP_LOWER_THRESHOLD;
    greenhouse_config.fan_temp_higher_threshold_C = DEFAULT_FAN_TEMP_HIGHER_THRESHOLD;
    greenhouse_config.pump_soilmoist_lower_threshold_pct = DEFAULT_PUMP_SOILMOIST_LOWER_THRESHOLD;
    greenhouse_config.growlight_light_threshold_pct = DEFAULT_GROWLIGHT_LIGHT_THRESHOLD;
    greenhouse_config.growlight_override = false;
    greenhouse_config.growlight_override_state = false;
    greenhouse_config.pump_override = false;
    greenhouse_config.pump_override_state = false;
    greenhouse_config.fan_override = false;
    greenhouse_config.fan_override_state = false;
}