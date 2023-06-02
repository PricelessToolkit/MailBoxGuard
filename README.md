## LoRa MailBox Sensor and LoRa Gateway



==============================
## LoRa_Gateway

### BOARD PINS
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


### First

- Change the BOARD definition in `board.h` according to the your board Version " 1 = ON / 0 = OFF ".
    ```
    #define LORA_V1_0_OLED  0
    #define LORA_V1_2_OLED  0
    #define LORA_V1_6_OLED  0
    #define LORA_V2_0_OLED  1
    ```
