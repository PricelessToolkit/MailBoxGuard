// Youtube Channel https://www.youtube.com/@PricelessToolkit/videos
// GitHub https://github.com/PricelessToolkit/MailBoxGuard

#include <SPI.h>
#include <LoRa.h>
#include <avr/sleep.h>


//////////////////////////////////// CONFIG /////////////////////////////////////////////

#define SignalBandwidth 125E3
#define SpreadingFactor 12
#define TransmitBattPercent 1
#define CodingRate 8
#define SyncWord 0xF3
#define PreambleLength 8
#define TxPower 20
#define BAND 868E6     // frequency in Hz (ASIA 433E6, EU 868E6, US 915E6)
String NewMailCode = "REPLACE_WITH_NEW_MAIL_CODE"; // For Example "0xA2B2";
String LowBatteryCode = "REPLACE_WITH_LOW_BATTERY_CODE"; // For Example "0xLBAT";

/////////////////////////////////////////////////////////////////////////////////////////

int loopcounter = 0;

void setup() {
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  Serial.begin(9600);   //Pin6 RX Pin7 TX
  analogReference(VDD);
  delay (5);


  if (!LoRa.begin(BAND)) {
    Serial.println("LoRaError");
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


void loop() {
	
  float volts = analogReadEnh(PIN_PB4, 12)*(1.1/4096)*(30+10)/10;
  Serial.println(volts);
  

  if (loopcounter < 2){
    delay(50);
    LoRa.beginPacket();
    if(TransmitBattPercent){
      float perc = map(volts * 1000, 3600, 4200, 0, 100);
      LoRa.print(NewMailCode + "," + perc);
    } else{
      LoRa.print(NewMailCode);
    }
    LoRa.endPacket();
    delay (10);

   }

  if (volts < 3.36 and loopcounter == 1 ){   // Don't change "3.36" !!
    LoRa.beginPacket();
    LoRa.print(LowBatteryCode);
    LoRa.endPacket();
    delay(50);
  }

  if(loopcounter > 2){
    //LoRa.end();  // Stop the LoRa library.
    LoRa.sleep();  // Put the radio in sleep mode
    digitalWrite(3, LOW); // Sets the Latch pin 3 LOW For power cut off
    
    /* Set sleep mode to PWR_DOWN */
    // There are five different sleep modes in order of power saving:
    // SLEEP_MODE_IDLE - the lowest power saving mode
    // SLEEP_MODE_ADC
    // SLEEP_MODE_PWR_SAVE
    // SLEEP_MODE_STANDBY
    // SLEEP_MODE_PWR_DOWN - the highest power saving mode
    
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_mode();  // Now enter sleep mode.
  }

  loopcounter ++;
  delay(10);
}
