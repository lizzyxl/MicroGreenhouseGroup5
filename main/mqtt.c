#include "mqtt.h"

#include "mqtt_client.h"
#include "esp_log.h"
#include "esp_crt_bundle.h"
#include "parameter_config.h"

static const char *TAG = "MQTT";

static esp_mqtt_client_handle_t s_client = NULL;
static bool s_connected = false;

/* These symbols come from EMBED_TXTFILES paths:
   main/certs/ca.pem, esp32.crt, esp32.key
*/
extern const uint8_t _binary_ca_pem_start[];
extern const uint8_t _binary_ca_pem_end[];

extern const uint8_t _binary_esp32_crt_start[];
extern const uint8_t _binary_esp32_crt_end[];

extern const uint8_t _binary_esp32_key_start[];
extern const uint8_t _binary_esp32_key_end[];

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        s_connected = true;
        ESP_LOGI(TAG, "Connected");
        esp_mqtt_client_subscribe(s_client, "greenhouse/config", 1);
        ESP_LOGI(TAG, "Subscribed to MQTT");
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "Topic: %.*s Data: %.*s", event->topic_len, event->topic, event->data_len, event->data);
        
        if (strncmp(event->topic, "greenhouse/config", event->topic_len) == 0) {
            parse_json_to_config(event->data, event->data_len);
        }
        break;

    case MQTT_EVENT_DISCONNECTED:
        s_connected = false;
        break;

    case MQTT_EVENT_ERROR:
        s_connected = false;
        ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
        break;

    default:
        break;
    }
}

esp_err_t mqtt_start(void)
{
    const char *uri = "mqtts://greenhouse-mqtt.norwayeast-1.ts.eventgrid.azure.net:8883";

    esp_mqtt_client_config_t cfg = {
        .broker.address.uri = uri,

        // Server verification (trust Azure broker cert chain)
        .broker.verification.crt_bundle_attach = esp_crt_bundle_attach,

        // Client auth (mutual TLS)
        .credentials.client_id = "esp32",
        .credentials.username  = "esp32",

        .credentials.authentication.certificate = (const char *)_binary_esp32_crt_start,
        .credentials.authentication.key = (const char *)_binary_esp32_key_start,
    };

    s_client = esp_mqtt_client_init(&cfg);
    if (!s_client) return ESP_FAIL;

    esp_mqtt_client_register_event(s_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);

    esp_err_t err = esp_mqtt_client_start(s_client);
    if (err != ESP_OK) return err;

    return ESP_OK;
}

bool mqtt_is_connected(void)
{
    return s_connected;
}

esp_err_t mqtt_publish(const char *topic, const char *payload, int qos, int retain)
{
    if (!s_client || !s_connected) return ESP_FAIL;

    int msg_id = esp_mqtt_client_publish(s_client, topic, payload, 0, qos, retain);
    return (msg_id >= 0) ? ESP_OK : ESP_FAIL;
}