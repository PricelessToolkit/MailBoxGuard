## LoRa MailBox Sensor and LoRa Gateway



==============================
## LoRa Gateway Configuration


### Select Board Version

- Change the BOARD definition in `board.h` according to the your board Version " 1 = ON / 0 = OFF ".
    ```
    #define LORA_V1_0_OLED  0
    #define LORA_V1_2_OLED  0
    #define LORA_V1_6_OLED  0
    #define LORA_V2_0_OLED  1
    ```
### LoRa Configuration

- The settings in the gateway and in the sensor must match.
```
#define SignalBandwidth 125E3
#define SpreadingFactor 12
#define CodingRate 8
#define SyncWord 0xF3
#define PreambleLength 8
#define TxPower 20
float BAND = 868E6; // 433E6 / 868E6 / 915E6 /
```

### WiFi Configuration
- Only in "LoRa_Gateway_MQTT.ino" and "LoRa_Gateway_WhatsApp.ino"
```
const char* ssid = "Your_WIFI_SSID";
const char* password = "Your_WIFI_password";

```
### MQTT Configuration

- Only in "LoRa_Gateway_MQTT.ino"
```
const char* mqtt_username = "Your_mqtt_username";
const char* mqtt_password = "Your_mqtt_password";
const char* mqtt_server = "Your_mqtt/homeassistant server IP";
const int mqtt_port = 1883;

```
