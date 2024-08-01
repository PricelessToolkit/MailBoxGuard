## Gateway code

For the TTGO_LoRa board, you can use the following firmwares:

- LoRa_Gateway_MQTT_Simple
- LoRa_Gateway_OLED
- LoRa_Gateway_WhatsApp

To pair these gateways with a sensor, use the sensor code `Mailbox_Guard_Sensor_Simple.ino`

## Arduino IDE Setup

> [!NOTE]
>  For the Library "megaTinyCore" the official recommendation is. Only versions of the Arduino IDE downloaded from https://arduino.cc should be used, NEVER from a Linux package manager. The golden standard is "V1.8.13".

- Additional Boards Manager URLs

  - ESP32 https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json
  - megaTinyCore http://drazzy.com/package_drazzy.com_index.json

- Libraries Used
  - LoRa
  - PubSubClient
  - ArduinoJson.h
  - ESP8266 and ESP32 OLED driver for SSD1306 displays
  - UrlEncode
  - HARestAPI "Sketch > Include Library > Add .ZIP Library..."
    - https://github.com/debsahu/HARestAPI
  
## Gateway Configuration

> [!NOTE]
>  If you are using CapiBridge Gateway, configure it using the [Capibridge](https://github.com/PricelessToolkit/CapiBridge) page.

## Select TTGO_LoRa Board Version

- In Arduino IDE select Tools > Board > ESP32 Arduino > `ESP32 Dev Module`
- Change the BOARD definition in `board.h` according to your Lilygo gateway HW Version " 1 = ENABLE / 0 = DISABLE ".

```c
#define LORA_V1_0_OLED  0
#define LORA_V1_2_OLED  0
#define LORA_V1_3_OLED  0
#define LORA_V1_6_OLED  0
#define LORA_V2_0_OLED  1
```

### TTGO Boards GPIOs

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


## Mailbox Sensor configuration and Programming

For programming MailBox Guard, you need any 3.3V "UPDI programmer" You can use my other open-source project "UNIProg Programmer" [GitHub](https://github.com/PricelessToolkit/UNIProg_Programmer)

### UPDI Connection Diagram

| UNIProg | MailBox Guard |
| ------- | ------------- |
| GND     | GND           |
| 3v3     | 3v3           |
| UPD     | UPD           |

1. In Arduino IDE select the programmer "SerialUPDI-230400 baud"
2. Select board configuration as shown below "screenshot"

<img src="https://raw.githubusercontent.com/PricelessToolkit/MailBoxGuard/main/img/arduino_board_config.jpg"  width="600" height="398" />

3. Open Sensor firmware `Mailbox_Guard_Sensor_Simple` then change  `NewMailCode`, `LowBatteryCode`, and `LoRa` settings according to your needs.

> [!NOTE]
> The LoRa settings must match the gateway ones.

```c
//////////////////////////////////// CONFIG /////////////////////////////////////////////

#define SignalBandwidth 125E3
#define SpreadingFactor 12
#define CodingRate 8
#define SyncWord 0xF3
#define PreambleLength 8
#define TxPower 20
#define BAND 868E6     // frequency in Hz (ASIA 433E6, EU 868E6, US 915E6)
String NewMailCode = "REPLACE_WITH_NEW_MAIL_CODE"; // For Example "0xA2B2";
String LowBatteryCode = "REPLACE_WITH_LOW_BATTERY_CODE"; // For Example "0xLBAT";

/////////////////////////////////////////////////////////////////////////////////////////

4. Click "Upload Using Programmer" or "Ctrl + Shift + U"

```
