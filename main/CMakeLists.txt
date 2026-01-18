idf_component_register(
    SRCS
        "main.c"
        "wifi.c"
        "mqtt.c"
        "soil_moisture.c"
    INCLUDE_DIRS "." "include"
    EMBED_TXTFILES
        "certs/ca.pem"
        "certs/esp32.crt"
        "certs/esp32.key"
)