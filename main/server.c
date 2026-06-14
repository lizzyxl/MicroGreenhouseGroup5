#include "server.h"
#include "esp_http_server.h"
#include "cJSON.h"
#include "esp_log.h"

#define TAG "SERVER"

static server_context_t server_ctx;

// GET data
esp_err_t data_get_handler(httpd_req_t *req) {
    // response with json formated measurements
    server_context_t *ctx = (server_context_t*)req->user_ctx;
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "temperature_C", ctx->measurements->temperature);
    cJSON_AddNumberToObject(root, "relative_humidity_pct", ctx->measurements->relative_humidity);
    cJSON_AddNumberToObject(root, "soil_moisture_pct", ctx->measurements->soil_moisture);
    cJSON_AddNumberToObject(root, "light_intensity_pct", ctx->measurements->light);
    
    char *json_str = cJSON_Print(root);
    httpd_resp_set_type(req, "application/json");
    
    esp_err_t ret = httpd_resp_send(req, json_str, strlen(json_str));
    free(json_str);
    cJSON_Delete(root);
    return ret;
}

// GET config
esp_err_t config_get_handler(httpd_req_t *req) {
    // responds with json formated config
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "measurement_interval_s", greenhouse_config.measurement_interval_s);
    cJSON_AddNumberToObject(root, "fan_temp_lower_threshold_C", greenhouse_config.fan_temp_lower_threshold_C);
    cJSON_AddNumberToObject(root, "fan_temp_higher_threshold_C", greenhouse_config.fan_temp_higher_threshold_C);
    cJSON_AddNumberToObject(root, "fan_hum_lower_threshold_pct", greenhouse_config.fan_hum_lower_threshold_pct);
    cJSON_AddNumberToObject(root, "fan_hum_higher_threshold_pct", greenhouse_config.fan_hum_higher_threshold_pct);
    cJSON_AddNumberToObject(root, "pump_soilmoist_threshold_pct", greenhouse_config.pump_soilmoist_threshold_pct);
    cJSON_AddNumberToObject(root, "growlight_light_threshold_pct", greenhouse_config.growlight_light_threshold_pct);
    cJSON_AddBoolToObject(root, "growlight_override", greenhouse_config.growlight_override);
    cJSON_AddBoolToObject(root, "growlight_override_state", greenhouse_config.growlight_override_state);
    cJSON_AddBoolToObject(root, "pump_override", greenhouse_config.pump_override);
    cJSON_AddBoolToObject(root, "pump_override_state", greenhouse_config.pump_override_state);
    cJSON_AddBoolToObject(root, "fan_override", greenhouse_config.fan_override);
    cJSON_AddBoolToObject(root, "fan_override_state", greenhouse_config.fan_override_state);

    char *json_str = cJSON_Print(root);
    httpd_resp_set_type(req, "application/json");

    esp_err_t ret = httpd_resp_send(req, json_str, strlen(json_str));
    free(json_str);
    cJSON_Delete(root);
    return ret;
}

// POST config
esp_err_t config_post_handler(httpd_req_t *req) {
    // parse received json into config for esp
    char buf[512] = {0};
    int ret = httpd_req_recv(req, buf, sizeof(buf)-1);

    if (ret <= 0) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    buf[ret] = '\0';
    
    ESP_LOGI(TAG, "Config POST: %s", buf);
    parse_json_to_config(buf, strlen(buf));
    
    httpd_resp_send(req, "{\"status\":\"updated\"}", -1);
    return ESP_OK;
}

// homepage
esp_err_t homepage_handler(httpd_req_t *req) {
    // show nice hompage with measurements and two buttons to interact with greenhouse (see index.html)
    extern const unsigned char index_html_start[] asm("_binary_index_html_start"); // creates pointer of the start of the binary of index.html
    extern const unsigned char index_html_end[] asm("_binary_index_html_end");
    
    httpd_resp_set_type(req, "text/html; charset=UTF-8");
    httpd_resp_set_hdr(req, "Cache-Control", "no-cache"); // no cache for automatic refresh
    
    size_t html_size = index_html_end - index_html_start;
    return httpd_resp_send(req, (const char *)index_html_start, html_size);
}

static const httpd_uri_t uri_homepage = {
    .uri      = "/",
    .method   = HTTP_GET,
    .handler  = homepage_handler,
    .user_ctx = NULL,
};
static const httpd_uri_t uri_data = {
    .uri      = "/data",
    .method   = HTTP_GET,
    .handler  = data_get_handler,
    .user_ctx = &server_ctx,
};
static const httpd_uri_t uri_config_get = {
    .uri      = "/config",
    .method   = HTTP_GET,
    .handler  = config_get_handler,
    .user_ctx = &server_ctx,
};
static const httpd_uri_t uri_config_post = {
    .uri      = "/config",
    .method   = HTTP_POST,
    .handler  = config_post_handler,
    .user_ctx = NULL,
};

void http_server_start(greenhouse_config_t *greenhouse_config, measurements_t *measurment) {
    //start webserver and register URI handlersi
    server_ctx.measurements = measurment;
    server_ctx.config = greenhouse_config;

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_start(&server, &config);

    httpd_register_uri_handler(server, &uri_homepage);
    httpd_register_uri_handler(server, &uri_data);
    httpd_register_uri_handler(server, &uri_config_get);
    httpd_register_uri_handler(server, &uri_config_post);
}

    

