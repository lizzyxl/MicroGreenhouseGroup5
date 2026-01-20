#define PIN_NUM_SDA               21 // SDA GPIO
#define PIN_NUM_SCL               22 // SCL GPIO
#define RED_WIFI_LED              GPIO_NUM_12
#define BUTTON_PIN_WHITE          GPIO_NUM_13
#define BUTTON_PIN_BLUE           GPIO_NUM_32
#define GROW_LIGHT_GPIO           GPIO_NUM_25
#define FAN_GPIO                  GPIO_NUM_26
#define DHT_GPIO                  GPIO_NUM_27
#define PUMP_GPIO                 GPIO_NUM_15
#define MOISTURE_LED              GPIO_NUM_17
#define SOIL_ADC_CHANNEL          ADC1_CHANNEL_1   // GPIO37
#define ADC_CHANNEL               ADC_CHANNEL_9    // GPIO33 for LDR
#define ADC_UNIT                  ADC_UNIT_2       // ADC2 for LDR

#define I2C_MASTER_SCL_IO GPIO_NUM_22 /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO GPIO_NUM_21 /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_0      /*!< I2C port number for master dev */