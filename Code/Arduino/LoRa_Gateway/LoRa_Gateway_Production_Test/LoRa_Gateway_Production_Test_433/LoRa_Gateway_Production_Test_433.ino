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
float BAND = 433E6;





String NewMailCode = "0xAAAA";
String LowBatteryCode = "0xFFFF";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

String recv;
int count = 0;
int ledState = LOW;  // ledState used to set the LED
int NewMailDetected = 0;
const long interval = 1000;  // interval at which to blink (milliseconds)
unsigned long previousMillis = 0;  // will store last time LED was updated




SSD1306Wire display(0x3C, 21, 22);



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
  
  digitalWrite(LED_PIN, LOW);

      BAND = 433E6;
      display.flipScreenVertically();
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 5, "PricelessToolkit");
      display.setFont(ArialMT_Plain_16);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 20, "MailBox Guard");
      display.drawString(64, 38, "RX 433MHz");
      display.display();
      delay(2000);
      display.clear();
      display.display();
      


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
