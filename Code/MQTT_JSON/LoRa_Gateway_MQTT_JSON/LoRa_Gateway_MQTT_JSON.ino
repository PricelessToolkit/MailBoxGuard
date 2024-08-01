#include <Arduino.h>
#include "board.h"
#include "config.h"
#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "SSD1306Wire.h"

SSD1306Wire display(OLED_ADDRESS, OLED_SDA, OLED_SCL);

#define BAUD 115200

#define MQTT_RETAIN true
#define BINARY_SENSOR_TOPIC "homeassistant/binary_sensor/"
#define SENSOR_TOPIC "homeassistant/sensor/"
#define LoRaGateway_RSSI_TOPIC "homeassistant/sensor/LoRaGateway/rssi"

unsigned long LedStartTime = 0;
unsigned long diagTimer = 60000;
unsigned long lastDiagTimer = 0;
unsigned long currentDiagMillis;

struct json_msg {
  String k;
  String id;
  int16_t r;
  int16_t b;
  int16_t v;
  float pw;
  int16_t l;
  String m;
  float w;
  String s;
  int16_t e;
  float t;
  float t2;
  int16_t hu;
  int16_t mo;
  String rw;
  String bt;
  float atm;
  float cd;
  String dr;
  String wd;
  String vb;
};

json_msg received_json_message;

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
    if (client.setBufferSize(1024)) {
      Serial.println("Buffer Size increased to 1024 byte");
    } else {
      Serial.println("Failed to allocate larger buffer");
    }

    String client_id = "LoRaGateway";

    if (client.connect(client_id.c_str(), MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("MQTT connected");

      Serial.println("Buffersize: " + client.getBufferSize());


        
    } else {
      Serial.print("MQTT failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1 seconds");
      delay(1000);
    }
  }
}



void setup() {
  Serial.begin(115200);

  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 5, "PricelessToolkit");
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 20, "LiLyGateway");
  display.drawString(64, 38, "RX " + String(BAND));
  display.display();
  delay(2000);
  display.clear();
  display.display();



  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  reconnect();
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);   // HIGH = OFF

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
  LoRa.setSyncWord(SYNC_WORD);  // byte value to use as the sync word, defaults to 0x12
  LoRa.setPreambleLength(PREAMBLE_LENGTH);  // Supported values are between 6 and 65535.
  LoRa.disableCrc();  // Enable or disable CRC usage, by default a CRC is not used LoRa.disableCrc() / LoRa.enableCrc();;
  LoRa.setTxPower(TX_POWER);  // TX power in dB, defaults to 17, Supported values are 2 to 20

}



void publishIfKeyExists(const JsonDocument& doc, const char* key, const String& topicSuffix) {
  if (doc.containsKey(key)) {
    String topic = String(SENSOR_TOPIC) + String(received_json_message.id) + topicSuffix;
    String binTopic = String(BINARY_SENSOR_TOPIC) + String(received_json_message.id) + topicSuffix;
    String value;

    // Determine the type of the key value and convert to string appropriately
    if (doc[key].is<int>()) {
      // For integers
      value = String(doc[key].as<int>());
    } else if (doc[key].is<float>()) {
      // For floats
      value = String(doc[key].as<float>(), 2); // Assuming 2 decimal places for float
    } else if (doc[key].is<String>()) {
      // For strings
      value = doc[key].as<String>();
    } else {
      // If the type is not expected, you can log an error or handle accordingly
      Serial.println("Unsupported type for key: " + String(key));
      return;
    }


 ///////////////////////// auto-discovery ////////////////////////////////



    // auto-discovery for Battery
    if (doc.containsKey("b")) {
        

    client.publish(
        (String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/batt/config").c_str(),
        (String("{"
        "\"name\":\"Battery\","
        "\"device_class\":\"battery\","
        "\"unit_of_measurement\":\"%\","
        "\"icon\":\"mdi:battery\","
        "\"entity_category\":\"diagnostic\","
        "\"state_topic\":\"") + String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/batt" + "\","
        "\"unique_id\":\"" + String(received_json_message.id).c_str() + "_batt" +"\","
        "\"device\":{\"identifiers\":[\"" + String(received_json_message.id).c_str() + "\"],"
        "\"name\":\"" + String(received_json_message.id).c_str() +"\","
        "\"mdl\":\"" + String(received_json_message.id).c_str() +
        "\",\"mf\":\"PricelessToolkit\"}}").c_str(),
        MQTT_RETAIN);

      }

    // auto-discovery for RSSI
    if (doc.containsKey("r")) {


    client.publish(
      (String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/rssi/config").c_str(),
      (String("{"
        "\"name\":\"RSSI\","
        "\"unit_of_measurement\":\"dBm\","
        "\"device_class\":\"signal_strength\","
        "\"icon\":\"mdi:signal\","
        "\"entity_category\":\"diagnostic\","
        "\"state_topic\":\"") + String(SENSOR_TOPIC) + String(received_json_message.id).c_str() +  "/rssi" +"\","
        "\"unique_id\":\"" + String(received_json_message.id).c_str() + "_rssi" +"\","
        "\"device\":{\"identifiers\":[\"" + String(received_json_message.id).c_str() + "\"],"
        "\"name\":\"" + String(received_json_message.id).c_str() + "\"}}").c_str(),
      MQTT_RETAIN);

      }

    
    // auto-discovery for Row data TEXT
    if (doc.containsKey("rw")) {


    client.publish(
      (String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/row/config").c_str(),
      (String("{"
        "\"name\":\"Text\","
        "\"icon\":\"mdi:text\","
        "\"state_topic\":\"") + String(SENSOR_TOPIC) + String(received_json_message.id).c_str() +  "/row" +"\","
        "\"unique_id\":\"" + String(received_json_message.id).c_str() + "_row" +"\","
        "\"device\":{\"identifiers\":[\"" + String(received_json_message.id).c_str() + "\"],"
        "\"name\":\"" + String(received_json_message.id).c_str() +"\","
        "\"mdl\":\"" + String(received_json_message.id).c_str() +
        "\",\"mf\":\"PricelessToolkit\"}}").c_str(),
      MQTT_RETAIN);

      }


    // auto-discovery for State
    if (doc.containsKey("s")) {


    client.publish(
      (String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/state/config").c_str(),
      (String("{"
        "\"name\":\"State\","
        "\"icon\":\"mdi:list-status\","
        "\"state_topic\":\"") + String(SENSOR_TOPIC) + String(received_json_message.id).c_str() +  "/state" +"\","
        "\"unique_id\":\"" + String(received_json_message.id).c_str() + "_state" +"\","
        "\"device\":{\"identifiers\":[\"" + String(received_json_message.id).c_str() + "\"],"
        "\"name\":\"" + String(received_json_message.id).c_str() + "\"}}").c_str(),
      MQTT_RETAIN);

      }

    // auto-discovery for Volt
    if (doc.containsKey("v")) {
        

    client.publish(
        (String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/volt/config").c_str(),
        (String("{"
        "\"name\":\"Volt\","
        "\"device_class\":\"voltage\","
        "\"unit_of_measurement\":\"V\","
        "\"icon\":\"mdi:flash-triangle\","
        "\"state_topic\":\"") + String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/volt" + "\","
        "\"unique_id\":\"" + String(received_json_message.id).c_str() + "_volt" +"\","
        "\"device\":{\"identifiers\":[\"" + String(received_json_message.id).c_str() + "\"],"
        "\"name\":\"" + String(received_json_message.id).c_str() +"\","
        "\"mdl\":\"" + String(received_json_message.id).c_str() +
        "\",\"mf\":\"PricelessToolkit\"}}").c_str(),
        MQTT_RETAIN);

      }


    // auto-discovery for Current
    if (doc.containsKey("pw")) {
        

    client.publish(
        (String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/current/config").c_str(),
        (String("{"
        "\"name\":\"Current\","
        "\"device_class\":\"current\","
        "\"unit_of_measurement\":\"mA\","
        "\"icon\":\"mdi:current-dc\","
        "\"state_topic\":\"") + String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/current" + "\","
        "\"unique_id\":\"" + String(received_json_message.id).c_str() + "_pw" +"\","
        "\"device\":{\"identifiers\":[\"" + String(received_json_message.id).c_str() + "\"],"
        "\"name\":\"" + String(received_json_message.id).c_str() +"\","
        "\"mdl\":\"" + String(received_json_message.id).c_str() +
        "\",\"mf\":\"PricelessToolkit\"}}").c_str(),
        MQTT_RETAIN);

      }




    // auto-discovery for illuminance
    if (doc.containsKey("l")) {
        

    client.publish(
        (String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/lx/config").c_str(),
        (String("{"
        "\"name\":\"Lux\","
        "\"device_class\":\"illuminance\","
        "\"unit_of_measurement\":\"lx\","
        "\"icon\":\"mdi:brightness-1\","
        "\"state_topic\":\"") + String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/lx" + "\","
        "\"unique_id\":\"" + String(received_json_message.id).c_str() + "_lx" +"\","
        "\"device\":{\"identifiers\":[\"" + String(received_json_message.id).c_str() + "\"],"
        "\"name\":\"" + String(received_json_message.id).c_str() +"\","
        "\"mdl\":\"" + String(received_json_message.id).c_str() +
        "\",\"mf\":\"PricelessToolkit\"}}").c_str(),
        MQTT_RETAIN);

      }



    // auto-discovery for Weight
    if (doc.containsKey("w")) {
        

    client.publish(
        (String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/weight/config").c_str(),
        (String("{"
        "\"name\":\"Weight\","
        "\"device_class\":\"weight\","
        "\"unit_of_measurement\":\"g\","
        "\"icon\":\"mdi:weight\","
        "\"state_topic\":\"") + String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/weight" + "\","
        "\"unique_id\":\"" + String(received_json_message.id).c_str() + "_w" +"\","
        "\"device\":{\"identifiers\":[\"" + String(received_json_message.id).c_str() + "\"],"
        "\"name\":\"" + String(received_json_message.id).c_str() +"\","
        "\"mdl\":\"" + String(received_json_message.id).c_str() +
        "\",\"mf\":\"PricelessToolkit\"}}").c_str(),
        MQTT_RETAIN);

      }


    // auto-discovery for Temperature
    if (doc.containsKey("t")) {
        

    client.publish(
        (String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/tmp/config").c_str(),
        (String("{"
        "\"name\":\"Temperature\","
        "\"device_class\":\"temperature\","
        "\"unit_of_measurement\":\"°C\","
        "\"icon\":\"mdi:thermometer\","
        "\"state_topic\":\"") + String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/tmp" + "\","
        "\"unique_id\":\"" + String(received_json_message.id).c_str() + "_tmp" +"\","
        "\"device\":{\"identifiers\":[\"" + String(received_json_message.id).c_str() + "\"],"
        "\"name\":\"" + String(received_json_message.id).c_str() +"\","
        "\"mdl\":\"" + String(received_json_message.id).c_str() +
        "\",\"mf\":\"PricelessToolkit\"}}").c_str(),
        MQTT_RETAIN);

      }


    // auto-discovery for Temperature 2
    if (doc.containsKey("t2")) {
        

    client.publish(
        (String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/tmp2/config").c_str(),
        (String("{"
        "\"name\":\"Temperature2\","
        "\"device_class\":\"temperature\","
        "\"unit_of_measurement\":\"°C\","
        "\"icon\":\"mdi:thermometer\","
        "\"state_topic\":\"") + String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/tmp2" + "\","
        "\"unique_id\":\"" + String(received_json_message.id).c_str() + "_tmp2" +"\","
        "\"device\":{\"identifiers\":[\"" + String(received_json_message.id).c_str() + "\"],"
        "\"name\":\"" + String(received_json_message.id).c_str() +"\","
        "\"mdl\":\"" + String(received_json_message.id).c_str() +
        "\",\"mf\":\"PricelessToolkit\"}}").c_str(),
        MQTT_RETAIN);

      }

    // auto-discovery for Humidity
    if (doc.containsKey("hu")) {
        

    client.publish(
        (String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/humidity/config").c_str(),
        (String("{"
        "\"name\":\"Humidity\","
        "\"device_class\":\"humidity\","
        "\"unit_of_measurement\":\"%\","
        "\"icon\":\"mdi:water-percent\","
        "\"state_topic\":\"") + String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/humidity" + "\","
        "\"unique_id\":\"" + String(received_json_message.id).c_str() + "_hu" +"\","
        "\"device\":{\"identifiers\":[\"" + String(received_json_message.id).c_str() + "\"],"
        "\"name\":\"" + String(received_json_message.id).c_str() +"\","
        "\"mdl\":\"" + String(received_json_message.id).c_str() +
        "\",\"mf\":\"PricelessToolkit\"}}").c_str(),
        MQTT_RETAIN);

      }

    // auto-discovery for Moisture
    if (doc.containsKey("mo")) {
        

    client.publish(
        (String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/moisture/config").c_str(),
        (String("{"
        "\"name\":\"Moisture\","
        "\"device_class\":\"Moisture\","
        "\"unit_of_measurement\":\"%\","
        "\"icon\":\"mdi:water-percent\","
        "\"state_topic\":\"") + String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/moisture" + "\","
        "\"unique_id\":\"" + String(received_json_message.id).c_str() + "_mo" +"\","
        "\"device\":{\"identifiers\":[\"" + String(received_json_message.id).c_str() + "\"],"
        "\"name\":\"" + String(received_json_message.id).c_str() +"\","
        "\"mdl\":\"" + String(received_json_message.id).c_str() +
        "\",\"mf\":\"PricelessToolkit\"}}").c_str(),
        MQTT_RETAIN);

      }


    // auto-discovery for Button
    if (doc.containsKey("bt")) {
        

    client.publish(
        (String(BINARY_SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/button/config").c_str(),
        (String("{"
        "\"name\":\"Button\","
        "\"device_class\":\"none\","
        "\"icon\":\"mdi:button\","
        "\"state_topic\":\"") + String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/button" + "\","
        "\"unique_id\":\"" + String(received_json_message.id).c_str() + "_bt" +"\","
        "\"payload_on\":\"on\","
        "\"payload_off\":\"off\","
        "\"device\":{\"identifiers\":[\"" + String(received_json_message.id).c_str() + "\"],"
        "\"name\":\"" + String(received_json_message.id).c_str() +"\","
        "\"mdl\":\"" + String(received_json_message.id).c_str() +
        "\",\"mf\":\"PricelessToolkit\"}}").c_str(),
        MQTT_RETAIN);

      }


    // auto-discovery for Atmospheric pressure
    if (doc.containsKey("atm")) {
        

    client.publish(
        (String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/pressure/config").c_str(),
        (String("{"
        "\"name\":\"Pressure\","
        "\"device_class\":\"atmospheric_pressure\","
        "\"unit_of_measurement\":\"kPa\","
        "\"icon\":\"mdi:button\","
        "\"state_topic\":\"") + String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/pressure" + "\","
        "\"unique_id\":\"" + String(received_json_message.id).c_str() + "_atm" +"\","
        "\"device\":{\"identifiers\":[\"" + String(received_json_message.id).c_str() + "\"],"
        "\"name\":\"" + String(received_json_message.id).c_str() +"\","
        "\"mdl\":\"" + String(received_json_message.id).c_str() +
        "\",\"mf\":\"PricelessToolkit\"}}").c_str(),
        MQTT_RETAIN);

      }


    // auto-discovery for CO2
    if (doc.containsKey("cd")) {

    client.publish(
        (String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/co2/config").c_str(),
        (String("{"
        "\"name\":\"Carbon Dioxide\","
        "\"device_class\":\"carbon_dioxide\","
        "\"unit_of_measurement\":\"ppm\","
        "\"icon\":\"mdi:molecule-co2\","
        "\"state_topic\":\"") + String(SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/co2" + "\","
        "\"unique_id\":\"" + String(received_json_message.id).c_str() + "_cd" +"\","
        "\"device\":{\"identifiers\":[\"" + String(received_json_message.id).c_str() + "\"],"
        "\"name\":\"" + String(received_json_message.id).c_str() +"\","
        "\"mdl\":\"" + String(received_json_message.id).c_str() +
        "\",\"mf\":\"PricelessToolkit\"}}").c_str(),
        MQTT_RETAIN);

      }



    // auto-discovery for Motion
    if (doc.containsKey("m")) {
        

    client.publish(
        (String(BINARY_SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/motion/config").c_str(),
        (String("{"
        "\"name\":\"Motion\","
        "\"device_class\":\"motion\","
        "\"icon\":\"mdi:motion\","
        "\"state_topic\":\"") + String(BINARY_SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/motion" + "\","
        "\"unique_id\":\"" + String(received_json_message.id).c_str() + "_m" +"\","
        "\"payload_on\":\"on\","
        "\"payload_off\":\"off\","
        "\"device\":{\"identifiers\":[\"" + String(received_json_message.id).c_str() + "\"],"
        "\"name\":\"" + String(received_json_message.id).c_str() +"\","
        "\"mdl\":\"" + String(received_json_message.id).c_str() +
        "\",\"mf\":\"PricelessToolkit\"}}").c_str(),
        MQTT_RETAIN);

      }


    // auto-discovery for door
    if (doc.containsKey("dr")) {

    client.publish(
        (String(BINARY_SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/door/config").c_str(),
        (String("{"
        "\"name\":\"Door\","
        "\"device_class\":\"door\","
        "\"icon\":\"mdi:door\","
        "\"state_topic\":\"") + String(BINARY_SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/door" + "\","
        "\"unique_id\":\"" + String(received_json_message.id).c_str() + "_door" +"\","
        "\"payload_on\":\"on\","
        "\"payload_off\":\"off\","
        "\"device\":{\"identifiers\":[\"" + String(received_json_message.id).c_str() + "\"],"
        "\"name\":\"" + String(received_json_message.id).c_str() +"\","
        "\"mdl\":\"" + String(received_json_message.id).c_str() +
        "\",\"mf\":\"PricelessToolkit\"}}").c_str(),
        MQTT_RETAIN);

      }


    // auto-discovery for window
    if (doc.containsKey("wd")) {

    client.publish(
        (String(BINARY_SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/window/config").c_str(),
        (String("{"
        "\"name\":\"Window\","
        "\"device_class\":\"window\","
        "\"icon\":\"mdi:window-closed\","
        "\"state_topic\":\"") + String(BINARY_SENSOR_TOPIC) + String(received_json_message.id).c_str()  + "/window" + "\","
        "\"unique_id\":\"" + String(received_json_message.id).c_str() + "_window" +"\","
        "\"payload_on\":\"on\","
        "\"payload_off\":\"off\","
        "\"device\":{\"identifiers\":[\"" + String(received_json_message.id).c_str() + "\"],"
        "\"name\":\"" + String(received_json_message.id).c_str() +"\","
        "\"mdl\":\"" + String(received_json_message.id).c_str() +
        "\",\"mf\":\"PricelessToolkit\"}}").c_str(),
        MQTT_RETAIN);

      }


    // auto-discovery for vibration
    if (doc.containsKey("vb")) {

    client.publish(
        (String(BINARY_SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/vibration/config").c_str(),
        (String("{"
        "\"name\":\"Vibration\","
        "\"device_class\":\"vibration\","
        "\"icon\":\"mdi:vibrate\","
        "\"state_topic\":\"") + String(BINARY_SENSOR_TOPIC) + String(received_json_message.id).c_str() + "/vibration" + "\","
        "\"unique_id\":\"" + String(received_json_message.id).c_str() + "_vibration" +"\","
        "\"payload_on\":\"on\","
        "\"payload_off\":\"off\","
        "\"device\":{\"identifiers\":[\"" + String(received_json_message.id).c_str() + "\"],"
        "\"name\":\"" + String(received_json_message.id).c_str() +"\","
        "\"mdl\":\"" + String(received_json_message.id).c_str() +
        "\",\"mf\":\"PricelessToolkit\"}}").c_str(),
        MQTT_RETAIN);

      }


 /////////////////////////////////////////////////////////////////////////////



    // Determine the topic based on the value of the key
    String state = doc[key].as<String>();
    if (state == "on" || state == "off") {
        // For binary sensors (with states like "on"/"off")
        client.publish(binTopic.c_str(), value.c_str(), MQTT_RETAIN);
    } else {
        // For regular sensors
        client.publish(topic.c_str(), value.c_str(), MQTT_RETAIN);
    }



  } else {

    //Serial.print("KEY not faund in JSON");

  }
}





void updateMessagesAndPublish(const JsonDocument& doc) {
  // Only proceed if GATEWAY_KEY matches
  if (!doc.containsKey("k") || doc["k"].as<String>() != GATEWAY_KEY) {
    Serial.println("Network Key Not Found or Invalid in JSON");
    return;
  }

  // Assuming GATEWAY_KEY matched and "id" is always required
  received_json_message.id = doc.containsKey("id") ? doc["id"].as<String>() : "";

  // Example usage for different keys | /Topics
  publishIfKeyExists(doc, "r", "/rssi");
  publishIfKeyExists(doc, "b", "/batt");
  publishIfKeyExists(doc, "v", "/volt");
  publishIfKeyExists(doc, "pw", "/current");
  publishIfKeyExists(doc, "l", "/lx");
  publishIfKeyExists(doc, "m", "/motion");
  publishIfKeyExists(doc, "w", "/weight");
  publishIfKeyExists(doc, "s", "/state");
  publishIfKeyExists(doc, "t", "/tmp");
  publishIfKeyExists(doc, "t2", "/tmp2");
  publishIfKeyExists(doc, "hu", "/humidity");
  publishIfKeyExists(doc, "mo", "/moisture");
  publishIfKeyExists(doc, "rw", "/row");
  publishIfKeyExists(doc, "bt", "/button");
  publishIfKeyExists(doc, "atm", "/pressur");
  publishIfKeyExists(doc, "cd", "/co2");
  publishIfKeyExists(doc, "dr", "/door");
  publishIfKeyExists(doc, "wd", "/window");
  publishIfKeyExists(doc, "vb", "/vibration");

  // Add other keys as needed...


}

void parseIncomingPacket(String serialrow) {
  StaticJsonDocument<1024> doc; // Adjust size as necessary

  DeserializationError error = deserializeJson(doc, serialrow);
  if (error) {
    Serial.print("deserializeJson() returned ");
    Serial.println(error.f_str());
    return;
  }

  updateMessagesAndPublish(doc);
}



void diag(){

  // Sending diagnostic data every 1m
  if (millis() -  lastDiagTimer >= diagTimer) { 
    long rssi = WiFi.RSSI();
 
    // send auto-discovery message for CapiBridge Diagnostic rssi
    client.publish(
      (String(LoRaGateway_RSSI_TOPIC) + String("/config")).c_str(),
      (String("{\"name\":\"RSSI\",\"unit_of_measurement\":\"dBm\",\"device_class\":\"signal_strength\",\"icon\":\"mdi:signal\",\"entity_category\":\"diagnostic\",\"state_topic\":\"") + String(LoRaGateway_RSSI_TOPIC) + String("\",\"unique_id\":\"LoRaGateway_rssi\",\"device\":{\"identifiers\":[\"LoRaGateway\"],\"name\":\"LoRaGateway\",\"mdl\":\"LoRaGateway\",\"mf\":\"PriclessToolkit\"}}")).c_str(),
      MQTT_RETAIN);

    // Sends RSSI value
    client.publish("homeassistant/sensor/LoRaGateway/rssi", String(rssi).c_str(), MQTT_RETAIN);

      lastDiagTimer = millis();
    }

}


void loop() {


  if (Serial.available() > 0) {
    // read the incoming string:
    String serialrow = Serial.readStringUntil('\n');
    parseIncomingPacket(serialrow);
    
  }


  if (LoRa.parsePacket()) {
    String recv = "";
    while (LoRa.available()) {
      recv += static_cast<char>(LoRa.read());
      digitalWrite(LED_PIN, HIGH);                 // Enabling LED_PIN
      LedStartTime = millis();                    // LED Timer Start
    }
    Serial.print("LoRa Message: ");
    
    if (client.connected()) {
      //////////////// Inserting RSSI ///////////////////
      // Prepare the string to insert
      String insertString = ",\"r\":" + String(LoRa.packetRssi());
      // Find the position of the last closing brace '}'
      int position = recv.lastIndexOf('}');
      // Assuming the position is valid and the JSON string is well-formed
      if (position != -1) {
          // Insert the new key-value pair before the last closing brace
          recv = recv.substring(0, position) + insertString + recv.substring(position);
          Serial.println(recv);
      }
      
      parseIncomingPacket(recv);
      
    }
   }

  if (millis() -  LedStartTime >= 100) {       // Turning OFF LED_PIN after 100ms
      digitalWrite(LED_PIN, LOW);             // HIGH = LED is OFF    
    }

  if (client.connected()) {
      diag();
    }

  if (!client.connected()) {
      reconnect();
    }
  client.loop();
}
