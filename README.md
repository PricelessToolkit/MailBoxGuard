<img src="https://raw.githubusercontent.com/PricelessToolkit/MailBoxGuard/main/img/mailbox_guard.jpg"/>

# Universal Long Range LoRa Sensor and Gateway
### Can be Integrated into Smart Homes or used offline.
### Use Cases
- Mailbox Sensor
- Motion Sensor
- Door Window Sensor

### How it works - Mailbox Use Case
The Mailbox Guard is a device that detects when a new letter or package has been delivered to your mailbox using a PIR sensor and door reed switch input. It can send a signal to your gateway, then gateway sends a message to `Home Assistant "MQTT"` , `WatsApp` allowing you to receive notifications directly into your phone*. Or you can use it offline, the gateway display will show number of letters received , Battery status and signal strength. For more information watch the YouTube video https://...

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

# MailBox Sensor Configuration

### MailBox LoRa Radio Configuration
- In `Mailbox_Guard_Sensor.ino`
- - The settings in the gateway and in the sensor must match.
```
  LoRa.setSignalBandwidth(125E3);         // signal bandwidth in Hz, defaults to 125E3
  LoRa.setSpreadingFactor(12);                 // ranges from 6-12,default 7 see API docs
  LoRa.setCodingRate4(8);        // Supported values are between 5 and 8, these correspond to coding rates of 4/5 and 4/8. The coding rate numerator is fixed at 4.
  LoRa.setSyncWord(0xF3);                     // byte value to use as the sync word, defaults to 0x12
  LoRa.setPreambleLength(8);       //Supported values are between 6 and 65535.
  LoRa.disableCrc();                          // Enable or disable CRC usage, by default a CRC is not used LoRa.disableCrc();
  LoRa.setTxPower(20);                // TX power in dB, defaults to 17, Supported values are 2 to 20
```

### New Mail and Low Battery Key
- In `Mailbox_Guard_Sensor.ino`
```
String NewMailCode = "REPLACE_WITH_NEW_MAIL_CODE"; // For Example "0xA2B2";
String LowBatteryCode = "REPLACE_WITH_LOW_BATTERY_CODE"; // For Example "0xLBAT";
```

### Frequency Configuration
- The settings in the gateway and in the sensor must match.
```
float BAND = 868E6; // 433E6 / 868E6 / 915E6 /
```
# Gateway Configuration

### Choosing Firmware for LoRa Gateway
1. `LoRa_Gateway_OLED.ino` "For offline use" Display turns on and shows that there is a new letter in the mailbox "number of letters", "signal strength" and "Battery State". After taking your mail, you need to press the reset button on the gateway.
2. `LoRa_Gateway_WhatsApp.ino` - `NOT TESTED YET` Sends a message to WhatsApp "You Have New Mail".
3. `LoRa_Gateway_MQTT.ino` Sends a row message and RSSI to MQTT Server.


### Select TTGO_LoRa Board Version

- Change the BOARD definition in `board.h` according to the your board Version " 1 = ON / 0 = OFF ".
 ```
 #define LORA_V1_0_OLED  0
 #define LORA_V1_2_OLED  0
 #define LORA_V1_6_OLED  0
 #define LORA_V2_0_OLED  1
 ```
 
 ## TTGO Boards GPIOs
| Name        | V1.0 | V1.2(T-Fox) | V1.6 | V2.0 |
| ----------- | ---- | ----------- | ---- | ---- |
| OLED RST    | 16   | N/A         | N/A  | N/A  |
| OLED SDA    | 4    | 21          | 21   | 21   |
| OLED SCL    | 15   | 22          | 22   | 22   |
| SDCard CS   | N/A  | N/A         | 13   | 13   |
| SDCard MOSI | N/A  | N/A         | 15   | 15   |
| SDCard MISO | N/A  | N/A         | 2    | 2    |
| SDCard SCLK | N/A  | N/A         | 14   | 14   |
| DS3231 SDA  | N/A  | 21          | N/A  | N/A  |
| DS3231 SCL  | N/A  | 22          | N/A  | N/A  |
| LORA MOSI   | 27   | 27          | 27   | 27   |
| LORA MISO   | 19   | 19          | 19   | 19   |
| LORA SCLK   | 5    | 5           | 5    | 5    |
| LORA CS     | 18   | 18          | 18   | 18   |
| LORA RST    | 14   | 23          | 23   | 23   |
| LORA DIO0   | 26   | 26          | 26   | 26   |
 
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
