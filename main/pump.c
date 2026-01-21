#include "pump.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "config.h"
#include "outputs.h"
#include "esp_timer.h"
#include "driver/gptimer.h"

#define TAG "PUMP_CONTROL"

#define PUMP_DURATION_MS 500 // Duration of the pump being on
#define WATERING_PAUSE_S 60

static gptimer_handle_t gptimer_oneshot = NULL;
static bool pump_state = false;
static bool previous_pump_state = false;
static volatile int64_t last_watering;

static bool IRAM_ATTR oneshot_callback_turn_pump_off(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx) {
    gptimer_stop(timer);
    gptimer_set_raw_count(timer, 0);
    gpio_set_level(PUMP_GPIO, 1);
    set_green_moisture_led(LED_OFF);  
    pump_state = false;
    return false;
}

void pump_init(void) {
    gpio_config_t io_conf_pump = {
        .pin_bit_mask = (1ULL << PUMP_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf_pump);
    gpio_set_level(PUMP_GPIO, 0);

    //oneshot timer
    gptimer_config_t oneshot_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000 * 1000,
    };
    gptimer_new_timer(&oneshot_config, &gptimer_oneshot);
    
    gptimer_event_callbacks_t oneshot_cbs = {
        .on_alarm = oneshot_callback_turn_pump_off,
    };
    gptimer_register_event_callbacks(gptimer_oneshot, &oneshot_cbs, NULL);
    
    gptimer_alarm_config_t oneshot_alarm = {
        .alarm_count = PUMP_DURATION_MS * 1000,
        .flags.auto_reload_on_alarm = false,
    };
    gptimer_set_alarm_action(gptimer_oneshot, &oneshot_alarm);
    gptimer_enable(gptimer_oneshot);
        
    ESP_LOGI(TAG, "Pump configured");
}

void pump_control(float moisture, float soilmoist_threshold_pct) {
    int64_t now = esp_timer_get_time();
    if (moisture <= soilmoist_threshold_pct && (last_watering - now) >= (WATERING_PAUSE_S * 1000)) { 
        last_watering = esp_timer_get_time();
        gpio_set_level(PUMP_GPIO, 0);
        set_green_moisture_led(LED_ON);  
        gptimer_start(gptimer_oneshot);
        pump_state = true;
        ESP_LOGI(TAG, "PUMP ON (Moisture below threshold)");
    }

    if (previous_pump_state == true && pump_state == false) {
        ESP_LOGI(TAG, "PUMP OFF");
    }
    previous_pump_state = pump_state;
}