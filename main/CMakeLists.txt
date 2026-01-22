idf_component_register(SRCS "i2c_init.c" "outputs.c" "soil_moisture.c" "temp_hum_sensor.c" "main.c" "fan.c" "pump.c" "display.c" "grow_light.c" "inputs.c" "light_sensor.c" "wifi.c" "mqtt.c" "adc_init.c" "parameter_config.c"
                    INCLUDE_DIRS "." "include"
                    EMBED_TXTFILES
                        "certs/ca.pem"
                        "certs/esp32.crt"
                        "certs/esp32.key")
