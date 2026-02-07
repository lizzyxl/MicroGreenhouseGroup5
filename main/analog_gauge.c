#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_cali.h"
#include <math.h>
#include "config.h"

#define TAG "ANALOG_GAUGE"

adc_oneshot_unit_handle_t adc2_handle;
adc_cali_handle_t cali_handle;

void analog_gauge_init(void) {
    // red
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_12_BIT, // 12-bit resolution (0–4096)
        .freq_hz = 50, // 50 Hz PWM frequency
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);
    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = ANALOG_GAUGE_GPIO,
        .duty = 494,
        .hpoint = 0
    };
    ledc_channel_config(&ledc_channel);
}

void analog_gauge_control(float percent) {
    int duty;
    duty = 494 - (percent * 392 / 100);

    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
}

/*
From SG90 Servo Datasheet:
20ms period (50Hz)
0.5 to 2.4ms duty cycle
Position "0" (1.45 ms pulse) is middle, "90" (～2.4 ms pulse) is all the way to the right, "-90" (～ 0.5 ms pulse) is all the way left.

LEDC 12-BIT Timer Settings:
Resolution = 12-bit → 4096 steps
(90° max left) 2.4ms/20ms = 12% -> Duty 494
(0° middle) 1.45ms/20ms = 7.25% -> Duty 298
(-90° max right) 0.5ms/20ms = 2.5% -> Duty 102 


*/