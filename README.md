<img src="https://raw.githubusercontent.com/PricelessToolkit/MailBoxGuard/main/img/mailbox_guard.jpg"/>

🤗 Please consider subscribing to my [YouTube channel](https://www.youtube.com/@PricelessToolkit/videos)
Your subscription goes a long way in backing my work.


[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/U6U2QLAF8)

# Long Range "LoRa" Universal MailBox Sensor

### Can be Integrated into Home Assistant, receive notifications via WhatsApp, or be used offline.

### Use Cases

- Mailbox Sensor - "Code examples are provided"
- Motion Sensor
- Door Window Sensor

### How it works - Mailbox Use Case

The Mailbox Guard is a device that detects when a new letter or package has been delivered to your mailbox using a PIR sensor and door reed switch. It can send a signal to your LoRa gateway, then the gateway sends a message via WiFi to `Home Assistant "MQTT" or its "API"` or to `WhatsApp` allowing you to receive notifications directly into your phone. Or you can use it offline, the gateway display will show the number of letters received, Battery status, and signal strength.

### Links

- YouTube video https://youtu.be/gf1WWKyEnbg
- My Shop

  - Mailbox Sensor [https://www.pricelesstoolkit.com/](https://www.pricelesstoolkit.com/en/projects/34-41-mailbox-guard-wireless-ir-sensor.html)
  - UNIProg Programmer [https://www.pricelesstoolkit.com/](https://www.pricelesstoolkit.com/en/projects/33-uniprog-uartupdi-programmer-33v.html)

- Gateway on Aliexpress [LILYGO® TTGO LoRa32 V2.1_1.6 Version 433/868/915Mhz](https://s.click.aliexpress.com/e/_DdCLj19)
- Gateway on official lilygo Shop [LILYGO® TTGO LoRa32 V2.1_1.6 Version 433/868/915Mhz](https://www.lilygo.cc/qcxgpu)
- Reed Switch "Normally open When the magnet is far away" "For soldering on PCB" https://s.click.aliexpress.com/e/_DkA1Sjp
- Reed Switch with cable "Normally open When the magnet is far away" https://s.click.aliexpress.com/e/_DFNJv1h
- Battery [Li-Ion 14500 800mA](https://s.click.aliexpress.com/e/_DDtU0iT)
- Battery Li-Ion 14500 1200mA - ?
- USB Battery charger "just in case" https://s.click.aliexpress.com/e/_DkDAztD

### Sensor Specifications

- Microcontroller "Attiny1616"
- LoRa Bands "433/868/915"
- PIR Sensor AM312 "With the option to turn it off"
- Onboard reed switch "optional"
- External reed switch "Input 2 Pin"
- Battery Holder for "Li-Ion 14500"
- Optional 2 Pin SMD LiPo Battery connector
- Build-in Battery Charger "Can be separated"
  - Connector USB-C
  - Charge status LED "Charging, Full"
- Extremely Low Power Consumption
  - Motion Sensor PIR 11.30uAh
  - Only reed switch 280nAh
  - Every data submission uses 65uA
  - TX LED "With the option to turn it off"
- Size
  - With charger "XXmm to XXmm"
  - Without charger "XXmm to XXmm"
- Programming Protocol
  - UPDI / Serial2UPDI

==============================

# ⚠️ Battery Polarity ⚠️

- Before connecting the battery, make sure to check the connector polarity on your battery. This is especially important because many 1S LiPo batteries have inverted polarity.

<img src="https://raw.githubusercontent.com/PricelessToolkit/MailBoxGuard/main/img/Polarity.jpg" width="600" height="340"/>

# Mailbox Sensor Programming

For programming MailBox Guard, you can use my other open-source project "UNIProg Programmer" [GitHub](https://github.com/PricelessToolkit/UNIProg_Programmer)

### Connection Diagram

| UNIProg | MailBox Guard |
| ------- | ------------- |
| GND     | GND           |
| 3v3     | 3v3           |
| UPD     | UPD           |

# Arduino Setup "IDE 2.0.x unsupported"

- Additional Boards Manager URLs

  - ESP32 https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json
  - megaTinyCore http://drazzy.com/package_drazzy.com_index.json

- Libraries Used
  - LoRa
  - PubSubClient
  - ESP8266 and ESP32 OLED driver for SSD1306 displays
  - UrlEncode
  - HARestAPI "Sketch > Include Library > Add .ZIP Library..."
    - https://github.com/debsahu/HARestAPI

# MailBox Sensor Configuration

<img src="https://raw.githubusercontent.com/PricelessToolkit/MailBoxGuard/main/img/arduino_board_config.jpg"  width="600" height="398" />

### Frequency Configuration

- In `Mailbox_Guard_Sensor.ino`
- The settings in the gateway and in the sensor must match.

```c
#define BAND 868E6 // frequency in Hz (ASIA 433E6, EU 868E6, US 915E6)
```

### New Mail and Low Battery Key

- In `Mailbox_Guard_Sensor.ino`

```c
String NewMailCode = "REPLACE_WITH_NEW_MAIL_CODE"; // For Example "0xA2B2";
String LowBatteryCode = "REPLACE_WITH_LOW_BATTERY_CODE"; // For Example "0xLBAT";
```

### MailBox LoRa Radio Configuration

- In `Mailbox_Guard_Sensor.ino`
  - The settings in the gateway and in the sensor must match.

```c
LoRa.setSignalBandwidth(125E3);         // signal bandwidth in Hz, defaults to 125E3
LoRa.setSpreadingFactor(12);            // ranges from 6-12, default 7 see API docs
LoRa.setCodingRate4(8);                 // Supported values are between 5 and 8, these correspond to coding rates of 4/5 and 4/8. The coding rate numerator is fixed at 4.
LoRa.setSyncWord(0xF3);                 // byte value to use as the sync word, defaults to 0x12
LoRa.setPreambleLength(8);              //Supported values are between 6 and 65535.
LoRa.disableCrc();                      // Enable or disable CRC usage, by default a CRC is not used LoRa.disableCrc();
LoRa.setTxPower(20);                    // TX power in dB, defaults to 17, Supported values are 2 to 20
```

# Gateway Configuration

### Choosing Firmware for LoRa Gateway

1. `LoRa_Gateway_OLED.ino` - "For offline use" Display turns on and shows that there is a new letter in the mailbox "the number of letters", "signal strength" and "Battery State". After taking your mail, you need to press the reset button on the gateway.
2. `LoRa_Gateway_WhatsApp.ino` - Sends a message to WhatsApp "You Have New Mail".
3. `LoRa_Gateway_MQTT.ino` - Sends a row message and RSSI to MQTT Server.
4. `LoRa_Gateway_HARestAPI.ino` - Sends a message to HA via the API interface.

### Select TTGO_LoRa Board Version

- In Arduino IDE select Tools > Board > ESP32 Arduino > `ESP32 Dev Module`
- Change the BOARD definition in `board.h` according to your gateway Version " 1 = ENABLE / 0 = DISABLE ".

```c
#define LORA_V1_0_OLED  0
#define LORA_V1_2_OLED  0
#define LORA_V1_3_OLED  0
#define LORA_V1_6_OLED  0
#define LORA_V2_0_OLED  1
```

## TTGO Boards GPIOs

| Name        | V1.0 | V1.2(T-Fox) | V1.3 | V1.6 | V2.1 |
| ----------- | ---- | ----------- | ---- | ---- | ---- |
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

```c
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

```c
String NewMailCode = "REPLACE_WITH_NEW_MAIL_CODE"; // For Example "0xA2B2";
String LowBatteryCode = "REPLACE_WITH_LOW_BATTERY_CODE"; // For Example "0xLBAT";
```

### WiFi Configuration

- In `LoRa_Gateway_MQTT.ino`, `LoRa_Gateway_HARestAPI.ino` and `LoRa_Gateway_WhatsApp.ino`

```c
const char* ssid = "Your_WIFI_SSID";
const char* password = "Your_WIFI_password";
```

### MQTT Configuration

- Only in `LoRa_Gateway_MQTT.ino`

```c
const char* mqtt_username = "Your_mqtt_username";
const char* mqtt_password = "Your_mqtt_password";
const char* mqtt_server = "Your_mqtt/homeassistant server IP";
const int mqtt_port = 1883;
```

## HARestAPI Configuration

- Open `LoRa_Gateway_HARestAPI.ino`
- For the `ha_pwd` go to your User Profile > Long-Lived Access Tokens > Create Token

```c
const char* ha_ip = "192.168.0.xxx";
uint16_t ha_port = 8123;
const char* ha_pwd = "HA_PASSWORD";
```

# Home Assistant Configuration

## MQTT

### Create a Sensor only For 'LoRa_Gateway_MQTT_Simple.ino', for 'LoRa_Gateway_MQTT_Auto_Discovery.ino' it's not required.

- File `loragateway.yaml`

```yaml
mqtt:
  sensor:
    - name: "LoRa_Code"
      state_topic: "LoRa-Gateway/Code"

    - name: "LoRa_RSSI"
      state_topic: "LoRa-Gateway/RSSI"
```

## HARestAPI

- File `configuration.yaml`

```yaml
input_boolean:
  mailbox_guard_motion:
    name: Mailbox Guard Motion
    icon: mdi:mail
  mailbox_guard_low_battery:
    name: Mailbox Guard Low Battery
    icon: mdi:battery

input_number:
  mailbox_guard_count:
    name: Mailbox Guard Count
    min: 0
    max: 255
    icon: mdi:mail
  mailbox_guard_battery:
    name: Mailbox Guard Battery
    min: 0
    max: 100
    icon: mdi:battery
  mailbox_guard_rssi:
    name: Mailbox Guard RSSI
    min: -196
    max: 63
    icon: mdi:signal
  mailbox_guard_snr:
    name: Mailbox Guard SNR
    min: -32.0
    max: 32.0
    icon: mdi:waves
```

- File `automation.yaml`

```yaml
- id: "1693702614043"
  alias: Mailbox Guard Motion
  description: ""
  trigger:
    - platform: state
      entity_id: input_boolean.mailbox_guard_motion
      to: "on"
  condition: []
  action:
    - service: notify.notify
      data:
        message: You've Got Mail!
    - service: input_number.set_value
      data:
        entity_id: input_number.mailbox_guard_count
        value: "{{ (states.input_number.mailbox_guard_count.state | int) + 1 }}"
    - service: input_boolean.turn_off
      data:
        entity_id: input_boolean.mailbox_guard_motion
  mode: single
- id: "1693703621871"
  alias: Mailbox Guard Low Battery
  description: ""
  trigger:
    - platform: state
      entity_id: input_boolean.mailbox_guard_low_battery
      to: "on"
  condition: []
  action:
    - service: notify.notify
      data:
        message: Mailbox Guard Low Battery!
    - service: input_boolean.turn_off
      data:
        entity_id: input_boolean.mailbox_guard_low_battery
  mode: single
```

- Dashboard card

```yaml
type: entities
title: Mailbox Guard
show_header_toggle: false
entities:
  - type: conditional
    conditions: []
    row:
      type: sensor-entity
      entity: input_number.mailbox_guard_count
      name: Motion Count
  - type: button
    icon: mdi:sync
    name: Reset Count
    action_name: RESET
    tap_action:
      action: call-service
      service: input_number.set_value
      data:
        entity_id: input_number.mailbox_guard_count
        value: 0
  - type: conditional
    conditions: []
    row:
      type: sensor-entity
      entity: input_number.mailbox_guard_battery
      name: Battery
  - type: conditional
    conditions: []
    row:
      type: sensor-entity
      entity: input_number.mailbox_guard_rssi
      name: RSSI
  - type: conditional
    conditions: []
    row:
      type: sensor-entity
      entity: input_number.mailbox_guard_snr
      name: SNR
```

### MailBox Guard motion detection events

1. Your phone receives a notification from HA `You've Got Mail!`
2. The MailBox Guard will add a count to the `Motion Count`
3. When you fetch the mail select the `RESET` button

![](/img/HARestAPI_Notify.png)

![](/img/HARestAPI.png)
