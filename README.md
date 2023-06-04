<img src="https://raw.githubusercontent.com/PricelessToolkit/MailBoxGuard/main/img/mailbox_guard.jpg"/>

# Long Range "LoRa" Universal MailBox Sensor
!!! Files for self-assembling will be after July !!!
### Can be Integrated into Smart Homes, Receive notifications via WhatsApp, or be used offline.
### Use Cases
- Mailbox Sensor - "Code examples are provided"
- Motion Sensor
- Door Window Sensor

### How it works - Mailbox Use Case
The Mailbox Guard is a device that detects when a new letter or package has been delivered to your mailbox using a PIR sensor and door reed switch. It can send a signal to your LoRa gateway, then the gateway sends a message via WiFi to `Home Assistant "MQTT"` or to `WhatsApp` allowing you to receive notifications directly into your phone. Or you can use it offline, the gateway display will show the number of letters received, Battery status, and signal strength.

### Links
- How To
- - YouTube video https://... "Not Ready"
- PricelessToolkit IoT Shop
- - MailBox Sensor [MailBox Guard](https://www.pricelesstoolkit.com/)
- - Serial2UPDI/ESP Programmer [UNIPROG](https://www.pricelesstoolkit.com/)
- Aliexpress
- - Gateway [LILYGO® TTGO LoRa32 V2.1_1.6 Version 433/868/915Mhz](https://s.click.aliexpress.com/e/_DCnmcvP)

### Sensor Specifications
- Microcontroller "Attiny1616"
- LoRa Bands "433/868/915"
- PIR Sensor AM312 "With option to turn it off"
- Onboard reed switch "optional"
- External reed switch "Input 2 Pin"
- Battery Holder for "Li-Ion 14500"
- Optional 2 Pin SMD LiPo Battery connector
- Buid in Battry Charger "Can be separated"
- - Connector USB-C
- - Charge status LED "Charging,Full"
- Extremely Low Power Consumption
- - Motion Sensor PIR 11.30uAh
- - Only reed switch 0.23uAh
- - Every data submission usese 65uA
- - TX LED "With option to turn it off"
- Size
- - With charger "XXmm to XXmm"
- - Without charger "XXmm to XXmm"


==============================

# Arduino Setup "IDE 2.0.x unsupported"

- Additional Boards Manager URLs
- - ESP32 https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json
- - megaTinyCore http://drazzy.com/package_drazzy.com_index.json

- Libraries Used
- - LoRa
- - PubSubClient
- - SSD1306
- - UrlEncode

# MailBox Sensor Configuration
<img src="https://raw.githubusercontent.com/PricelessToolkit/MailBoxGuard/main/img/arduino_board_config.jpg"  width="600" height="398" />


### Frequency Configuration
- In `Mailbox_Guard_Sensor.ino`
- The settings in the gateway and in the sensor must match.
```
#define BAND 868E6 // frequency in Hz (ASIA 433E6, EU 868E6, US 915E6)
```

### New Mail and Low Battery Key
- In `Mailbox_Guard_Sensor.ino`
```
String NewMailCode = "REPLACE_WITH_NEW_MAIL_CODE"; // For Example "0xA2B2";
String LowBatteryCode = "REPLACE_WITH_LOW_BATTERY_CODE"; // For Example "0xLBAT";
```

### MailBox LoRa Radio Configuration
- In `Mailbox_Guard_Sensor.ino`
- - The settings in the gateway and in the sensor must match.
```
  LoRa.setSignalBandwidth(125E3);         // signal bandwidth in Hz, defaults to 125E3
  LoRa.setSpreadingFactor(12);            // ranges from 6-12,default 7 see API docs
  LoRa.setCodingRate4(8);                 // Supported values are between 5 and 8, these correspond to coding rates of 4/5 and 4/8. The coding rate numerator is fixed at 4.
  LoRa.setSyncWord(0xF3);                 // byte value to use as the sync word, defaults to 0x12
  LoRa.setPreambleLength(8);              //Supported values are between 6 and 65535.
  LoRa.disableCrc();                      // Enable or disable CRC usage, by default a CRC is not used LoRa.disableCrc();
  LoRa.setTxPower(20);                    // TX power in dB, defaults to 17, Supported values are 2 to 20
```

# Gateway Configuration

### Choosing Firmware for LoRa Gateway
1. `LoRa_Gateway_OLED.ino` "For offline use" Display turns on and shows that there is a new letter in the mailbox "number of letters", "signal strength" and "Battery State". After taking your mail, you need to press the reset button on the gateway.
2. `LoRa_Gateway_WhatsApp.ino` - `NOT TESTED YET` Sends a message to WhatsApp "You Have New Mail".
3. `LoRa_Gateway_MQTT.ino` Sends a row message and RSSI to MQTT Server.


### Select TTGO_LoRa Board Version

- Change the BOARD definition in `board.h` according to the your board Version " 1 = ENABLE / 0 = DISABLE ".
 ```
 #define LORA_V1_0_OLED  0
 #define LORA_V1_2_OLED  0
 #define LORA_V1_3_OLED  0
 #define LORA_V1_6_OLED  0
 #define LORA_V2_0_OLED  1
 ```
 
 ## TTGO Boards GPIOs
| Name        | V1.0 | V1.2(T-Fox) | V1.3 | V1.6 | V2.0 |
| ----------- | ---- | ----------- |------| ---- | ---- |
| OLED RST    | 16   | N/A         | N/A  | N/A  | N/A  |
| OLED SDA    | 4    | 21          | 4    | 21   | 21   |
| OLED SCL    | 15   | 22          | 15   | 22   | 22   |
| SDCard CS   | N/A  | N/A         | N/A  | 13   | 13   |
| SDCard MOSI | N/A  | N/A         | N/A  | 15   | 15   |
| SDCard MISO | N/A  | N/A         | N/A  | 2    | 2    |
| SDCard SCLK | N/A  | N/A         | N/A  | 14   | 14   |
| DS3231 SDA  | N/A  | 21          | N/A  | N/A  | N/A  |
| DS3231 SCL  | N/A  | 22          | N/A  | N/A  | N/A  |
| LORA MOSI   | 27   | 27          | 27   | 27   | 27   |
| LORA MISO   | 19   | 19          | 19   | 19   | 19   |
| LORA SCLK   | 5    | 5           | 5    | 5    | 5    |
| LORA CS     | 18   | 18          | 18   | 18   | 18   |
| LORA RST    | 14   | 23          | 23   | 23   | 23   |
| LORA DIO0   | 26   | 26          | 26   | 26   | 26   |
 
### Gateway LoRa Radio Configuration

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
### New Mail Key Configuration

- In `LoRa_Gateway_OLED.ino` and `LoRa_Gateway_WhatsApp.ino`
```
String NewMailCode = "REPLACE_WITH_NEW_MAIL_CODE"; // For Example "0xA2B2";
String LowBatteryCode = "REPLACE_WITH_LOW_BATTERY_CODE"; // For Example "0xLBAT";
```


### WiFi Configuration
- In `LoRa_Gateway_MQTT.ino` and `LoRa_Gateway_WhatsApp.ino`
```
const char* ssid = "Your_WIFI_SSID";
const char* password = "Your_WIFI_password";
```
### MQTT Configuration

- Only in `LoRa_Gateway_MQTT.ino`
```
const char* mqtt_username = "Your_mqtt_username";
const char* mqtt_password = "Your_mqtt_password";
const char* mqtt_server = "Your_mqtt/homeassistant server IP";
const int mqtt_port = 1883;

```

# Home Assistant Configuration

### Create a Sensor
- File `loragateway.yaml`
```
mqtt:
    sensor:
     - name: "LoRa_Code"
       state_topic: "LoRa-Gateway/Code"

     - name: "LoRa_RSSI"
       state_topic: "LoRa-Gateway/RSSI"
```
