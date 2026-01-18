#pragma once
#include "esp_err.h"
#include <stdbool.h>

esp_err_t wifi_init_sta(const char *ssid, const char *pass);
bool wifi_is_connected(void);
