#include "secrets.h"
#include "board.h"
#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "SSD1306Wire.h"
#include <ArduinoJson.h>
SSD1306Wire display(OLED_ADDRESS, OLED_SDA, OLED_SCL);

/////////////////////////////////// CONFIG ////////////////////////////////////

#define SIGNAL_BANDWITH 125E3
#define SPREADING_FACTOR 7
#define CODING_RATE 5
#define SYNC_WORK 0xF3
#define PREAMBLE_LENGTH 8
#define TX_POWER 1
#define BAND 433E6  // 433E6 / 868E6 / 915E6

#define MQTT_RETAIN true

#define MQTT_MAILBOX_TOPIC "homeassistant/binary_sensor/mailbox/state"
#define MQTT_BAT_TOPIC "homeassistant/sensor/mailbox/battery"
#define MQTT_RSSI_TOPIC "homeassistant/sensor/mailbox/rssi"

#define NEW_MAIL_CODE "NEWMAIL"

///////////////////////////////////////////////////////////////////////////////

struct lora_msg {
  String message;
  float bat_voltage;
  int16_t packet_rssi;
};

lora_msg lora_message;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Buffer needs to be increased to accomodate the config payloads
    if (client.setBufferSize(380)) {
      Serial.println("Buffer Size increased to 380 byte");
    } else {
      Serial.println("Failed to allocate larger buffer");
    }

    String client_id = "LoRaGateway-";
    client_id += String(random(0xffff), HEX);

    if (client.connect(client_id.c_str(), MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("MQTT connected");

      Serial.println("Buffersize: " + client.getBufferSize());

      // send auto-discovery message for mail status sensor
      client.publish(
        (String(MQTT_MAILBOX_TOPIC) + String("/config")).c_str(),
        (String("{\"name\":null,\"device_class\":\"door\",\"icon\":\"mdi:mailbox\",\"state_topic\":\"") + String(MQTT_MAILBOX_TOPIC) + String("\",\"unique_id\":\"mailbox_sensor\",\"payload_on\":\"mail\",\"payload_off\":\"empty\",\"device\":{\"identifiers\":[\"mailbox\"],\"name\":\"Mailbox\",\"mdl\":\"MAILBOXguard+\",\"mf\":\"PricelessToolkit\"}}")).c_str(),
        MQTT_RETAIN);
      // send auto-discovery message for rssi
      client.publish(
        (String(MQTT_RSSI_TOPIC) + String("/config")).c_str(),
        (String("{\"name\":\"RSSI\",\"unit_of_measurement\":\"dBm\",\"device_class\":\"signal_strength\",\"icon\":\"mdi:signal\",\"entity_category\":\"diagnostic\",\"state_topic\":\"") + String(MQTT_RSSI_TOPIC) + String("\",\"unique_id\":\"mailbox_signal\",\"device\":{\"identifiers\":[\"mailbox\"],\"name\":\"Mailbox\"}}")).c_str(),
        MQTT_RETAIN);
      // send auto-discovery message for battery voltage
      client.publish(
        (String(MQTT_BAT_TOPIC) + String("/config")).c_str(),
        (String("{\"name\":\"Battery\",\"unit_of_measurement\":\"V\",\"device_class\":\"battery\",\"icon\":\"mdi:battery\",\"entity_category\":\"diagnostic\",\"state_topic\":\"") + String(MQTT_BAT_TOPIC) + String("\",\"unique_id\":\"mailbox_battery_voltage\",\"device\":{\"identifiers\":[\"mailbox\"],\"name\":\"Mailbox\"}}")).c_str(),
        MQTT_RETAIN);
    } else {
      Serial.print("MQTT failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1 seconds");
      delay(1000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  reconnect();

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
    while (1) {}
  }

  LoRa.setSignalBandwidth(SIGNAL_BANDWITH);  // signal bandwidth in Hz, defaults to 125E3
  LoRa.setSpreadingFactor(SPREADING_FACTOR);  // ranges from 6-12,default 7 see API docs
  LoRa.setCodingRate4(CODING_RATE);  // Supported values are between 5 and 8, these correspond to coding rates of 4/5 and 4/8. The coding rate numerator is fixed at 4.
  LoRa.setSyncWord(SYNC_WORK);  // byte value to use as the sync word, defaults to 0x12
  LoRa.setPreambleLength(PREAMBLE_LENGTH);  // Supported values are between 6 and 65535.
  LoRa.disableCrc();  // Enable or disable CRC usage, by default a CRC is not used LoRa.disableCrc();
  LoRa.setTxPower(TX_POWER);  // TX power in dB, defaults to 17, Supported values are 2 to 20
}

void parsePacket(String rawpkg) {
  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, rawpkg);

  if (error) {
    Serial.print("deserializeJson() returned ");
    Serial.println(error.f_str());
    return;
  }

  if (doc.containsKey("message")) {
    lora_message.message = doc["message"].as<String>();
    Serial.print("message: ");
    Serial.println(lora_message.message);
  } else {
    lora_message.message = "";
    Serial.println("no message in json");
  }

  if (doc.containsKey("bat_voltage")) {
    lora_message.bat_voltage = doc["bat_voltage"].as<float>();
    Serial.print("bat_voltage: ");
    Serial.println(lora_message.bat_voltage);
  } else {
    lora_message.bat_voltage = -1.0;
    Serial.println("no battery voltage (bat_voltage) in json");
  }

  lora_message.packet_rssi = LoRa.packetRssi();
}

void loop() {
  if (LoRa.parsePacket()) {
    String recv = "";
    while (LoRa.available()) {
      recv += static_cast<char>(LoRa.read());
    }
    Serial.println(recv);

    if (client.connected()) {
      parsePacket(recv);

      if (lora_message.message == NEW_MAIL_CODE) {
        client.publish(MQTT_MAILBOX_TOPIC, "mail", MQTT_RETAIN);
      }

      client.publish(MQTT_RSSI_TOPIC, String(lora_message.packet_rssi).c_str(), MQTT_RETAIN);
      client.publish(MQTT_BAT_TOPIC, String(lora_message.bat_voltage).c_str(), MQTT_RETAIN);

      client.endPublish();

      lora_message.message = "";
      lora_message.bat_voltage = -1.0;
      lora_message.packet_rssi = -1;
    }
  }

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
