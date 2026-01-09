#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include "esp_err.h"
#include "esp_log.h"
#include <driver/i2c_master.h>
#include <driver/gpio.h>
#include <lvgl.h>
#include <esp_lcd_panel_vendor.h>
#include <sys/param.h>
#include "display.h"
#include "utils.h"

#define TAG "DISPLAY"

#define I2C_BUS_PORT           0
#define LCD_PIXEL_CLOCK_HZ     (400 * 1000)
#define PIN_NUM_SDA            21 // SDA GPIO
#define PIN_NUM_SCL            22 // SCL GPIO
#define PIN_NUM_RST            -1
#define I2C_HW_ADDR            0x3C // OLED address
#define LCD_H_RES              128 //Screen width
#define LCD_V_RES              64 // Screen height
#define LCD_CMD_BITS           8
#define LCD_PARAM_BITS         8
#define LVGL_TICK_PERIOD_MS    5
#define LVGL_PALETTE_SIZE      8

static uint8_t oled_buffer[LCD_H_RES * LCD_V_RES / 8]; //Raw pixel buffer in bytes
static _lock_t lvgl_api_lock;
static lv_display_t *greenhouse_display;
static esp_lcd_panel_handle_t panel_handle;
static esp_lcd_panel_io_handle_t io_handle;
static esp_timer_handle_t lvgl_tick_timer;

#define GAME_PANEL_WIDTH 128
#define GAME_PANEL_HEIGHT 48
#define MAX_SCREENS 4

// lvgl handles
static lv_obj_t *display_screen = NULL;
static lv_obj_t *blue_panel = NULL;
static lv_obj_t *yellow_panel = NULL;
static lv_obj_t *measurment_description_label = NULL;
static lv_obj_t *measurment_value_label = NULL;

static bool notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t io_panel, esp_lcd_panel_io_event_data_t *edata, void *user_ctx) {
    lv_display_flush_ready((lv_display_t*)user_ctx);
    return false;
}

static void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)lv_display_get_user_data(disp);
    
    px_map += LVGL_PALETTE_SIZE;
    uint16_t hor_res = lv_display_get_physical_horizontal_resolution(disp);
    int x1 = area->x1, x2 = area->x2, y1 = area->y1, y2 = area->y2;

    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) {
            bool chroma_color = (px_map[(hor_res >> 3) * y + (x >> 3)] & (1 << (7 - x % 8)));
            uint8_t *buf = oled_buffer + hor_res * (y >> 3) + x;
            if (chroma_color) {
                (*buf) |= (1 << (y % 8));
            } else {
                (*buf) &= ~(1 << (y % 8));
            }
        }
    }
    esp_lcd_panel_draw_bitmap(panel_handle, x1, y1, x2 + 1, y2 + 1, oled_buffer);
}

static void increase_lvgl_tick(void *arg) {
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

static void lvgl_task(void *arg) {
    uint32_t time_till_next = 0;
    while (1) {
        _lock_acquire(&lvgl_api_lock);
        time_till_next = lv_timer_handler();
        _lock_release(&lvgl_api_lock);
        
        uint32_t delay = MAX(time_till_next, 5);
        vTaskDelay(pdMS_TO_TICKS(delay));
    }
}

void greenhouse_display_init(void) {
    i2c_master_bus_handle_t i2c_bus = NULL;
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .i2c_port = I2C_BUS_PORT,
        .sda_io_num = PIN_NUM_SDA,
        .scl_io_num = PIN_NUM_SCL,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus));

    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = I2C_HW_ADDR,
        .scl_speed_hz = LCD_PIXEL_CLOCK_HZ,
        .control_phase_bytes = 1,
        .lcd_cmd_bits = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
        .dc_bit_offset = 6,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = PIN_NUM_RST,
    };
    esp_lcd_panel_ssd1306_config_t ssd1306_config = {
        .height = LCD_V_RES,
    };
    panel_config.vendor_config = &ssd1306_config;
    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    lv_init();
    
    greenhouse_display = lv_display_create(LCD_H_RES, LCD_V_RES);
    lv_display_set_user_data(greenhouse_display, panel_handle);

    size_t draw_buffer_sz = LCD_H_RES * LCD_V_RES / 8 + LVGL_PALETTE_SIZE;
    void *buf = heap_caps_calloc(1, draw_buffer_sz, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(buf);
    
    lv_display_set_color_format(greenhouse_display, LV_COLOR_FORMAT_I1);
    lv_display_set_buffers(greenhouse_display, buf, NULL, draw_buffer_sz, LV_DISP_RENDER_MODE_FULL);
    lv_display_set_flush_cb(greenhouse_display, lvgl_flush_cb);

    const esp_lcd_panel_io_callbacks_t cbs = {
        .on_color_trans_done = notify_lvgl_flush_ready,
    };
    esp_lcd_panel_io_register_event_callbacks(io_handle, &cbs, greenhouse_display);

    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000));

    xTaskCreate(lvgl_task, "LVGL", 4096, NULL, 2, NULL);
}

// ui_init
void ui_init_once(void)
{
    static bool initialized = false;
    if (initialized) return;
    initialized = true;

    display_screen = lv_obj_create(NULL);
    lv_screen_load(display_screen);
    lv_obj_set_style_bg_color(display_screen, lv_color_black(), 0);

    // --- Yellow panel ---
    yellow_panel = lv_obj_create(display_screen);
    lv_obj_set_size(yellow_panel, 128, 16);
    lv_obj_set_pos(yellow_panel, 0, 48);
    lv_obj_set_style_bg_color(yellow_panel, lv_color_black(), 0);
    lv_obj_clear_flag(yellow_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(yellow_panel, 0, 0);
    lv_obj_set_style_pad_row(yellow_panel, 0, 0);
    lv_obj_set_style_pad_column(yellow_panel, 0, 0);
    lv_obj_set_style_border_width(yellow_panel, 0, 0);

    // --- Blue panel ---
    blue_panel = lv_obj_create(display_screen);
    lv_obj_set_size(blue_panel, 128, 48);
    lv_obj_set_pos(blue_panel, 0, 0);
    lv_obj_set_style_bg_color(blue_panel, lv_color_black(), 0);
    lv_obj_set_style_pad_all(blue_panel, 0, 0);
    lv_obj_set_style_pad_row(blue_panel, 0, 0);
    lv_obj_set_style_pad_column(blue_panel, 0, 0);
    lv_obj_set_style_border_width(blue_panel, 3, 3);

    // Measurement description
    measurment_description_label = lv_label_create(blue_panel);
    lv_obj_set_size(measurment_description_label, 122, 12);
    lv_obj_set_pos(measurment_description_label, 3, 8);
    lv_obj_set_style_text_align(measurment_description_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(measurment_description_label, lv_color_white(), 0);
    lv_obj_set_style_bg_color(measurment_description_label, lv_color_black(), 0);
    lv_obj_set_style_text_font(measurment_description_label, &lv_font_unscii_8, 0);

    // Measurement value
    measurment_value_label = lv_label_create(blue_panel);
    lv_obj_set_size(measurment_value_label, 122, 12);
    lv_obj_set_pos(measurment_value_label, 3, 26); 
    lv_obj_set_style_text_align(measurment_value_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(measurment_value_label, lv_color_white(), 0);
    lv_obj_set_style_bg_color(measurment_value_label, lv_color_black(), 0);
    lv_obj_set_style_text_align(measurment_value_label, LV_TEXT_ALIGN_CENTER, 0); 
    lv_obj_set_style_text_font(measurment_value_label, &lv_font_unscii_16, 0);
}

void display_draw(measurements_t *measurements, bool button_press) {
    _lock_acquire(&lvgl_api_lock);
    ui_init_once();

    static int cycle_screen = 0;

    if (button_press) {
        cycle_screen++;
        if (cycle_screen > MAX_SCREENS-1) {
            cycle_screen = 0;
        }
    }

    switch (cycle_screen) {
        case 0: //temp
            lv_label_set_text_fmt(measurment_description_label, "Temperature");
            lv_label_set_text_fmt(measurment_value_label, "%dC", (int)measurements->temperature);
            break;
        case 1: //relative humidtiy
            lv_label_set_text_fmt(measurment_description_label, "Relative Humidity");
            lv_label_set_text_fmt(measurment_value_label, "%d%%", (int)measurements->relative_humidity);
            break;
        case 2: //light
            lv_label_set_text_fmt(measurment_description_label, "Light Intensity");
            lv_label_set_text_fmt(measurment_value_label, "%d", measurements->light);
            break;
        case 3: //soil moisture
            lv_label_set_text_fmt(measurment_description_label, "Soil Moisture");
            lv_label_set_text_fmt(measurment_value_label, "%d", (int)measurements->soil_moisture);
            break;
        default:
            break;
    }

    _lock_release(&lvgl_api_lock);
}

