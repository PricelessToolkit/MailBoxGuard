#include "board.h"
#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <PubSubClient.h>
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
const char *mqtt_username = "Your_mqtt_username";
const char *mqtt_password = "Your_mqtt_password";
const char *mqtt_server = "Your_mqtt/homeassistant server IP";
const int mqtt_port = 1883;
bool retain = true;

String NewMailCode = "REPLACE_WITH_NEW_MAIL_CODE"; // For Example "0xA2B2";
String LowBatteryCode = "REPLACE_WITH_LOW_BATTERY_CODE"; // For Example "0xLBAT";

// IMPORTANT: Set TransmitBattPercent to 1 in the Sensor Config!

String bat_val = "";
String received_code = "";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

String recv;
int count = 0;

WiFiClient espClient;
PubSubClient client(espClient);

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

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Buffer needs to be increased to accomodate the config payloads
    if(client.setBufferSize(380)){
    Serial.println("Buffer Size increased to 380 byte"); 
    }else{
     Serial.println("Failed to allocate larger buffer");   
     }

    // Create a random client ID
    String clientId = "LoRaGateway-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("MQTT connected");

     Serial.println("Buffersize: " + client.getBufferSize());   

      
      // Send auto-discovery message for sensor
      client.publish(
        "homeassistant/binary_sensor/mailbox/config",
        "{\"name\":null,\"device_class\":\"door\",\"icon\":\"mdi:mailbox\",\"state_topic\":\"homeassistant/binary_sensor/mailbox/state\",\"unique_id\":\"mailbox_sensor\",\"payload_on\":\"mail\",\"payload_off\":\"empty\",\"device\":{\"identifiers\":[\"mailbox\"],\"name\":\"Mailbox\",\"mdl\":\"MAILBOXguard+\",\"mf\":\"PricelessToolkit\"}}",
        retain
      );
      
      // Send auto-discovery message for signal
      client.publish(
        "homeassistant/sensor/mailbox/rssi/config",
        "{\"name\":\"RSSI\",\"device_class\":\"signal_strength\",\"icon\":\"mdi:signal\",\"entity_category\":\"diagnostic\",\"state_topic\":\"homeassistant/sensor/mailbox/rssi\",\"unique_id\":\"mailbox_signal\",\"device\":{\"identifiers\":[\"mailbox\"],\"name\":\"Mailbox\"}}",
        retain
      );
       client.publish(
        "homeassistant/sensor/mailbox/batt/config",
        "{\"name\":\"Battery\",\"device_class\":\"battery\",\"icon\":\"mdi:battery\",\"entity_category\":\"diagnostic\",\"state_topic\":\"homeassistant/sensor/mailbox/batt\",\"unique_id\":\"mailbox_batt\",\"device\":{\"identifiers\":[\"mailbox\"],\"name\":\"Mailbox\"}}",
        retain
      );
       client.publish(
        "homeassistant/binary_sensor/mailbox/battlow/config",
        "{\"name\":\"Battery State\",\"device_class\":\"battery\",\"icon\":\"mdi:battery\",\"entity_category\":\"diagnostic\",\"state_topic\":\"homeassistant/binary_sensor/mailbox/battlow\",\"unique_id\":\"mailbox_battlow\",\"device\":{\"identifiers\":[\"mailbox\"],\"name\":\"Mailbox\"}}",
        retain
      );
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
  while (!Serial);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  reconnect();
  // client.setCallback(MQTTCallback);

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

void parsePacket(String rawpkg){
  int i = 0;
  int intconv;

  // Empty vals to be sure we have new data
  bat_val = "";
  received_code = "";
  
  while(rawpkg[i] != ',' && rawpkg[i] != '\0' ){
    i++;
  }

  received_code=rawpkg.substring(0,i);

  // "Converts" float to int - For cosmetic reasons. Modify, if desired
  bat_val=rawpkg.substring(i+1);
  intconv = bat_val.toFloat() + 0;
  bat_val = String(intconv) + " %";
  
  }

void loop() {
  if (LoRa.parsePacket()) {
    String recv = "";
    while (LoRa.available()) {
      recv += (char)LoRa.read();
    }

    Serial.println(recv);
    if (client.connected()) {
      parsePacket(recv);
      
      if(received_code == NewMailCode){
        client.publish("homeassistant/binary_sensor/mailbox/state", "mail", retain);
       };
       
      if(received_code == LowBatteryCode){
        client.publish("homeassistant/binary_sensor/mailbox/battlow", "ON", retain);
       } else {
        client.publish("homeassistant/binary_sensor/mailbox/battlow", "OFF", retain);
       };

       
        
      String rs = String(LoRa.packetRssi());
      client.publish("homeassistant/sensor/mailbox/rssi", rs.c_str(), retain);
      client.publish("homeassistant/sensor/mailbox/batt", bat_val.c_str(), retain);

      client.endPublish();
    }
  }

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
