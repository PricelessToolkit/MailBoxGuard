#include "board.h"
#include <SPI.h>
#include <LoRa.h>
#include "SSD1306Wire.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <UrlEncode.h>
SSD1306Wire display(OLED_ADDRESS, OLED_SDA, OLED_SCL);


///////////////////////////////////////////////// CHANGE THIS /////////////////////////////////////////////////////////////////

#define SignalBandwidth 125E3
#define SpreadingFactor 12
#define CodingRate 8
#define SyncWord 0xF3
#define PreambleLength 8
#define TxPower 20
float BAND = 868E6;  // 868E6 / 915E6 / 433E6

String NewMailCode = "REPLACE_WITH_NEW_MAIL_CODE"; // For Example "0xA2B2";
String LowBatteryCode = "REPLACE_WITH_LOW_BATTERY_CODE"; // For Example "0xLBAT";


const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// +international_country_code + phone number
// Portugal +351, example: +351912345678
String phoneNumber = "REPLACE_WITH_YOUR_PHONE_NUMBER";

/////////////////// API Key Setup //////////////////////////////////////
//You need to get the apikey form the bot before using the API:

// 1. Add the phone number +34 644 45 70 57 into your Phone Contacts. (Name it it as you wish)
// 2. Send this message "I allow callmebot to send me messages" to the new Contact created (using WhatsApp of course)
// 3. Wait until you receive the message "API Activated for your phone number. Your APIKEY is XXXXXX" from the bot.
// Note: If you don't receive the ApiKey in 2 minutes, please try again after 24hs.
// The WhatsApp message from the bot will contain the apikey needed to send messages using the API.

String apiKey = "REPLACE_WITH_API_KEY";


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////







String recv;


void sendMessage(String message){

  // Data to send with HTTP POST
  String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);    
  HTTPClient http;
  http.begin(url);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  // Send HTTP POST request
  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200){
    Serial.print("Message sent successfully");
  }
  else{
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();
}




void setup() {
  Serial.begin(9600);
  while (!Serial);

  
  if (OLED_RST > 0) {
      pinMode(OLED_RST, OUTPUT);
      digitalWrite(OLED_RST, HIGH);
      delay(100);
      digitalWrite(OLED_RST, LOW);
      delay(100);
      digitalWrite(OLED_RST, HIGH);
  }
  display.init();
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);


      display.flipScreenVertically();
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 5, "PricelessToolkit");
      display.setFont(ArialMT_Plain_16);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 20, "MailBox Guard");
      display.drawString(64, 38, "RX " + String(BAND));
      display.display();
      delay(2000);
      display.clear();
      display.display();
      


  SPI.begin(CONFIG_CLK, CONFIG_MISO, CONFIG_MOSI, CONFIG_NSS);
  LoRa.setPins(CONFIG_NSS, CONFIG_RST, CONFIG_DIO0);
  Serial.println("Starting LoRa on " + String(BAND)+ " MHz");
  if (!LoRa.begin(BAND)) {
      Serial.println("Starting LoRa failed!");
      while (1);
  }
  
  LoRa.setSignalBandwidth(SignalBandwidth);         // signal bandwidth in Hz, defaults to 125E3
  LoRa.setSpreadingFactor(SpreadingFactor);                 // ranges from 6-12,default 7 see API docs
  LoRa.setCodingRate4(CodingRate);        // Supported values are between 5 and 8, these correspond to coding rates of 4/5 and 4/8. The coding rate numerator is fixed at 4.
  LoRa.setSyncWord(SyncWord);                     // byte value to use as the sync word, defaults to 0x12
  LoRa.setPreambleLength(PreambleLength);       //Supported values are between 6 and 65535.
  LoRa.disableCrc();                          // Enable or disable CRC usage, by default a CRC is not used LoRa.disableCrc();
  LoRa.setTxPower(TxPower);                // TX power in dB, defaults to 17, Supported values are 2 to 20




  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // Send Message to WhatsAPP
  sendMessage("MailBox Gateway Is Connected to WIFI");



}



void loop(){
    
    if (LoRa.parsePacket()) {
        String recv = "";
        while (LoRa.available()) {
            recv += (char)LoRa.read();
        }
        
        Serial.println(recv);

        if (recv == NewMailCode){
            sendMessage("New Mail in The Mailbox");
         }
        
         if (recv == LowBatteryCode){
            sendMessage("MailBox Battery is LOW!");
         }
  

  }

}
