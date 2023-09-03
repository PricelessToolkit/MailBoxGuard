#include "board.h"
#include <SPI.h>
#include <LoRa.h>
#include <WiFiClient.h>
#include <HARestAPI.h>          // https://github.com/debsahu/HARestAPI
#include "SSD1306Wire.h"
SSD1306Wire display(OLED_ADDRESS, OLED_SDA, OLED_SCL);

///////////////////////////////////////////////////// CHANGE THIS /////////////////////////////////////////////////////////////

#define SignalBandwidth 125E3
#define SpreadingFactor 12
#define CodingRate 8
#define SyncWord 0xF3
#define PreambleLength 8
#define TxPower 20
float BAND = 868E6; // 433E6 / 868E6 / 915E6 /

const char *ssid = "Your_WIFI_SSID";
const char *password = "Your_WIFI_password";
const char* ha_ip = "homeassistant.local";
uint16_t ha_port = 8123;
const char* ha_pwd = "HA_PASSWORD";
String fingerprint = "35 85 74 EF 67 35 A7 CE 40 69 50 F3 C0 F6 80 CF 80 3B 2E 19";

String NewMailCode = "REPLACE_WITH_NEW_MAIL_CODE"; // For Example "0xA2B2";
String LowBatteryCode = "REPLACE_WITH_LOW_BATTERY_CODE"; // For Example "0xLBAT";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

String recv;

WiFiClient espClient;
HARestAPI client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  setup_wifi();
  client.setHAServer(ha_ip, ha_port);
  client.setHAPassword(ha_pwd);
  //client.setFingerPrint(fingerprint);
  client.setTimeOut(5000); // 5 seconds

  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 5, "PricelessToolkit");
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 20, "LoRa Gateway");
  display.drawString(64, 38, "RX " + String(BAND));
  display.display();
  delay(2000);
  display.clear();
  display.display();

  SPI.begin(CONFIG_CLK, CONFIG_MISO, CONFIG_MOSI, CONFIG_NSS);
  LoRa.setPins(CONFIG_NSS, CONFIG_RST, CONFIG_DIO0);
  Serial.println("Starting LoRa on " + String(BAND) + " MHz");
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  LoRa.setSignalBandwidth(SignalBandwidth); // signal bandwidth in Hz, defaults to 125E3
  LoRa.setSpreadingFactor(SpreadingFactor); // ranges from 6-12,default 7 see API docs
  LoRa.setCodingRate4(CodingRate);          // Supported values are between 5 and 8, these correspond to coding rates of 4/5 and 4/8. The coding rate numerator is fixed at 4.
  LoRa.setSyncWord(SyncWord);               // byte value to use as the sync word, defaults to 0x12
  LoRa.setPreambleLength(PreambleLength);   // Supported values are between 6 and 65535.
  LoRa.disableCrc();                        // Enable or disable CRC usage, by default a CRC is not used LoRa.disableCrc();
  LoRa.setTxPower(TxPower);                 // TX power in dB, defaults to 17, Supported values are 2 to 20
}

void loop() {
  if (LoRa.parsePacket()) {
    String recv = "";
    while (LoRa.available()) {
      recv += (char)LoRa.read();
    }

    String rssi = String(LoRa.packetRssi());
    String snr = String(LoRa.packetSnr());

    Serial.println(recv);
    Serial.println(rssi);
    Serial.println(snr);

    if (recv == NewMailCode) {
      client.sendHAComponent("/api/services/input_boolean/turn_on", "input_boolean.mailbox_guard_motion");
    }
    else if (recv == LowBatteryCode) {
      client.sendHAComponent("/api/services/input_boolean/turn_on", "input_boolean.mailbox_guard_low_battery");
    }
  }
}