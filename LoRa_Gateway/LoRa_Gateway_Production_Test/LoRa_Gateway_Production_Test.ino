#include <SPI.h>
#include <LoRa.h>
#include "SSD1306Wire.h" 



// Board LiLyGo_LoRa32_V2.1_1.6
#define OLED_RST  -1
#define LORA_MOSI 27
#define LORA_MISO 19
#define LORA_CLK  5
#define LORA_NSS  18
#define LORA_RST  23
#define LORA_DIO0 26
#define SDCARD_MOSI 15
#define SDCARD_MISO 2
#define SDCARD_SCLK 14
#define SDCARD_CS   13
#define BAND_PIN    4
#define LED_PIN    25

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SignalBandwidth 125E3
#define SpreadingFactor 12
#define CodingRate 8
#define SyncWord 0xF3
#define PreambleLength 8
#define TxPower 20
float BAND = 868E6;





String NewMailCode = "0xA2B2";
String LowBatteryCode = "0xLBAT";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

String recv;
int count = 0;
int BAND_MODE = 0;
int ledState = LOW;  // ledState used to set the LED
int NewMailDetected = 0;
const long interval = 1000;  // interval at which to blink (milliseconds)
unsigned long previousMillis = 0;  // will store last time LED was updated



String Band915 = ("915000000.00," + String(SignalBandwidth) + "," + String(SpreadingFactor) + "," + String(CodingRate) + "," + String(SyncWord) + "," + String(PreambleLength) + "," + String(TxPower));
String Band868 = ("868000000.00," + String(SignalBandwidth) + "," + String(SpreadingFactor) + "," + String(CodingRate) + "," + String(SyncWord) + "," + String(PreambleLength) + "," + String(TxPower));

SSD1306Wire display(0x3C, 21, 22);

#define Bitmap_mail_logo_width 62
#define Bitmap_mail_logo_height 40
const unsigned char Bitmap_mail_logo [] PROGMEM = {
  0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 
  0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x3f, 
  0xf7, 0x01, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x3b, 0xe7, 0x03, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x39, 
  0xc7, 0x07, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x38, 0x87, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x38, 
  0x07, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x38, 0x07, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x38, 
  0x07, 0xf8, 0x00, 0x00, 0x00, 0xc0, 0x07, 0x38, 0x07, 0xf0, 0x01, 0x00, 0x00, 0xe0, 0x03, 0x38, 
  0x07, 0xe0, 0x03, 0x00, 0x00, 0xf0, 0x01, 0x38, 0x07, 0xc0, 0x07, 0x00, 0x00, 0xf8, 0x00, 0x38, 
  0x07, 0x80, 0x0f, 0x00, 0x00, 0x7c, 0x00, 0x38, 0x07, 0x00, 0x3e, 0x00, 0x00, 0x3e, 0x00, 0x38, 
  0x07, 0x00, 0x7c, 0x00, 0x80, 0x0f, 0x00, 0x38, 0x07, 0x00, 0xf8, 0x00, 0xc0, 0x07, 0x00, 0x38, 
  0x07, 0x00, 0xf0, 0x01, 0xe0, 0x03, 0x00, 0x38, 0x07, 0x00, 0xe0, 0x03, 0xf0, 0x01, 0x00, 0x38, 
  0x07, 0x00, 0xc0, 0x07, 0xf8, 0x01, 0x00, 0x38, 0x07, 0x00, 0xf0, 0x0f, 0xfc, 0x03, 0x00, 0x38, 
  0x07, 0x00, 0xf8, 0x3e, 0xfe, 0x07, 0x00, 0x38, 0x07, 0x00, 0x7c, 0xfc, 0x8f, 0x0f, 0x00, 0x38, 
  0x07, 0x00, 0x3e, 0xf8, 0x07, 0x1f, 0x00, 0x38, 0x07, 0x00, 0x1f, 0xf0, 0x03, 0x3e, 0x00, 0x38, 
  0x07, 0x80, 0x0f, 0xe0, 0x01, 0xf8, 0x00, 0x38, 0x07, 0xe0, 0x03, 0xc0, 0x00, 0xf0, 0x01, 0x38, 
  0x07, 0xf0, 0x01, 0x00, 0x00, 0xe0, 0x03, 0x38, 0x07, 0xf8, 0x00, 0x00, 0x00, 0xc0, 0x07, 0x38, 
  0x07, 0x7c, 0x00, 0x00, 0x00, 0x80, 0x0f, 0x38, 0x07, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x38, 
  0x07, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x38, 0x87, 0x07, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x38, 
  0xe7, 0x03, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x39, 0xf7, 0x01, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x3b, 
  0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x3f, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 
  0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07
};



#define Bitmap_battery_full_width 27
#define Bitmap_battery_full_height 15
const unsigned char Bitmap_battery_full [] PROGMEM = {


  0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0x03, 0x00, 0x80, 0x00, 0x33, 0xe6, 0x9c, 0x03, 
  0x7b, 0xe7, 0x9c, 0x07, 0x7b, 0xe7, 0x9c, 0x07, 0x7b, 0xe7, 0x9c, 0x07, 0x7b, 0xe7, 0x9c, 0x07, 
  0x7b, 0xe7, 0x9c, 0x07, 0x7b, 0xe7, 0x9c, 0x07, 0x7b, 0xe7, 0x9c, 0x07, 0x33, 0xe6, 0x9c, 0x03, 
  0x03, 0x00, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00
  
};




#define Bitmap_battery_empty_width 27
#define Bitmap_battery_empty_height 15
const unsigned char Bitmap_battery_empty [] PROGMEM = {


  0xfe, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0x03, 0x00, 0x80, 0x00, 0x03, 0x00, 0x80, 0x01, 
  0x13, 0x00, 0x80, 0x07, 0x13, 0x00, 0x80, 0x07, 0x13, 0x00, 0x80, 0x07, 0x13, 0x00, 0x80, 0x07, 
  0x13, 0x00, 0x80, 0x07, 0x13, 0x00, 0x80, 0x07, 0x13, 0x00, 0x80, 0x07, 0x13, 0x00, 0x80, 0x03, 
  0x03, 0x00, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00, 0xfe, 0xff, 0xff, 0x00
  
};



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
  
  pinMode(BAND_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  BAND_MODE = digitalRead(BAND_PIN);
  if (BAND_MODE == HIGH){
      BAND = 915E6;
      Serial.println(Band915);
      display.flipScreenVertically();
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 5, "PricelessToolkit");
      display.setFont(ArialMT_Plain_16);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 20, "MailBox Guard");
      display.drawString(64, 38, "RX 915MHz");
      display.display();
      delay(2000);
      display.clear();
      display.display();
      
  }
  else
     {
      BAND = 868E6;
      Serial.println(Band868);
      display.flipScreenVertically();
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 5, "PricelessToolkit");
      display.setFont(ArialMT_Plain_16);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 20, "MailBox Guard");
      display.drawString(64, 38, "RX 868MHz");
      display.display();
      delay(2000);
      display.clear();
      display.display();
    
     }

  




  SPI.begin(LORA_CLK, LORA_MISO, LORA_MOSI, LORA_NSS);
  LoRa.setPins(LORA_NSS, LORA_RST, LORA_DIO0);
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
  
}




void NewMailLed()
{

   if (NewMailDetected == 1){
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        // save the last time you blinked the LED
        previousMillis = currentMillis;

        // if the LED is off turn it on and vice-versa:
        if (ledState == LOW) {
          ledState = HIGH;
      } else {
          ledState = LOW;
        }

        // set the LED with the ledState of the variable:
        digitalWrite(LED_PIN, ledState);
    }
  } 
}



void loop(){
  
    NewMailLed();
    
    if (LoRa.parsePacket()) {
        String recv = "";
        while (LoRa.available()) {
            recv += (char)LoRa.read();
        }
        
        count++;


////////////////////////////////////TEST///////////////////////////////////////////////

        digitalWrite(LED_PIN, HIGH);
        delay(5);
        digitalWrite(LED_PIN, LOW);

        String info = "CODE: " + String(recv);
        String rss = "RSSI: " + String(LoRa.packetRssi());
        String msg = "MSG: " + String(count);
        //Serial.println(rss);
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_10);
        display.drawString(5, 5, String(info));
        display.drawString(5, 20, String(rss));
        display.setFont(ArialMT_Plain_16);
        display.drawString(5, 35, String(msg));
        display.display();

        if (String(recv)== "0xLBAT"){
          display.setFont(ArialMT_Plain_16);
          display.drawString(90, 35, "[LB]");
          display.display();
        }

///////////////////////////////////////////////////////////////////////////////////////////


  }
}
