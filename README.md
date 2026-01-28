# MicroGreenhouse Project

This repository contains the ESP-IDF project template for the **MicroGreenhouse** developed by Group 5.  
The project is designed to monitor and control environmental conditions for a micro greenhouse using the ESP32 microcontroller. It includes support for multiple sensors and actuators (temperature, humidity, light, fans, water pumps, etc.) and is structured in a modular way using ESP-IDF components.

---

## Project Purpose

The goal of this project is to create a modular, maintainable framework for controlling and monitoring a micro greenhouse.  
Each sensor and actuator has its own component folder, allowing multiple team members to work independently and integrate their modules seamlessly.  
The project also aims to provide:

- Real-time monitoring of greenhouse conditions  
- Automatic control of actuators based on sensor data  
- Easy integration of new sensors and actuators  
- A clear structure for collaborative development

## MQTT/UART key value pairs
```text
"measurement_interval_s":int_value //sets time in seconds between each measurement cycle
"fan_temp_lower_threshold_C":float_value //set threshold when the fan should turn off again in C
"fan_temp_higher_threshold_C":float_value //set threshold when the fan should turn on in C
"fan_hum_lower_threshold_pct":float_value //set threshold when the fan should turn off again in %
"fan_hum_higher_threshold_pct":float_value //set threshold when the fan should turn on in %
"pump_soilmoist_threshold_pct":float_value //set threshold when the pump should turn on in %
"growlight_light_threshold_pct":float_value //set threshold when the growlight should turn on %
"fan_override":bool_value //set the fan to manual mode (true) or automatic (false)
"fan":bool_value //set the fan to manual mode and to on or off
"pump_override":bool_value //set the pump to manual mode (true) or automatic (false)
"pump":bool_value //set the pump to manual mode and to on or off
"growlight_override":bool_value //set the grow light to manual mode (true) or automatic (false)
"growlight":bool_value //set the growlight to manual mode and to on or off
"config":"default" // resets whole config to default values
"wifi_ssid":char_string //sets wifi name
"wifi_password":char_string // sets wifi password
// if one of these values is updated a measurement is executed imidiately
// setting a value eg. with {"fan":true} automatically turns override mode on
```
---

## How to use this project

1. Clone the repository to your local machine:

```bash
git clone https://github.com/lizzyxl/MicroGreenhouseGroup5.git
cd MicroGreenhouseGroup5
```

2. Create a new feature branch for your work:
```bash
git checkout -b feature-branch-name
```

3. Build and flash the project using ESP-IDF:
```bash
idf.py set-target esp32
idf.py menuconfig       # Optional: configure pins, Wi-Fi, etc.
idf.py build
idf.py flash
idf.py monitor
```
