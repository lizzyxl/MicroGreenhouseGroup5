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


---

## Project Structure

This document describes the folder and file organization for the **MicroGreenhouse** ESP-IDF project.

```text
MicroGreenhouseGroup5/
├── CMakeLists.txt             # Top-level CMake build configuration
├── main/
│   ├── CMakeLists.txt         # CMake configuration for the main application
│   └── main.c                 # Entry point for the project
├── components/                # Folder for all sensor/actuator components
│   ├── temp_sensor/           # Temperature sensor component
│   │   ├── temp_sensor.c      # Implementation
│   │   ├── temp_sensor.h      # Public interface
│   │   └── CMakeLists.txt     # Component build instructions
│   ├── humidity_sensor/       # Humidity sensor component
│   │   ├── humidity_sensor.c
│   │   ├── humidity_sensor.h
│   │   └── CMakeLists.txt
│   ├── light_sensor/          # Light sensor component
│   │   ├── light_sensor.c
│   │   ├── light_sensor.h
│   │   └── CMakeLists.txt
│   ├── fan_actuator/          # Fan actuator component
│   │   ├── fan_actuator.c
│   │   ├── fan_actuator.h
│   │   └── CMakeLists.txt
│   └── water_pump/            # Water pump actuator component
│       ├── water_pump.c
│       ├── water_pump.h
│       └── CMakeLists.txt
└── README.md                  # Project documentation
```


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

4. Develop new components by creating folders under `/components`.

### Guidelines for adding Components
1. Create new folder under `components/` for your sensor or actuator.
2. Include the following files:

- `component_name.c` -> Implementation
- `component_name.h` -> Public interface
- `CMakeLists.txt` -> Registers the component with ESP-IDF

3. Test your component independently before integrating into `main.c`
4. Follow consistent naming conventions (e.q., `sensor_<name>` or `actuator_<name>`)

