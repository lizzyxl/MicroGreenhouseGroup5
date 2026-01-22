#pragma once
#include "esp_err.h"
#include <stdbool.h>

esp_err_t wifi_init_sta(const char *ssid, const char *pass);
bool wifi_is_connected(void);
esp_err_t wifi_reconnect(void);
esp_err_t wifi_reconfigure(const char *new_ssid, const char *new_pass);