// Youtube Channel https://www.youtube.com/@PricelessToolkit/videos
// GitHub https://github.com/PricelessToolkit/MailBoxGuard

#include <SPI.h>
#include <LoRa.h>
#include <avr/sleep.h>

///////////////////////////////// CHANGE THIS /////////////////////////////////

#define SIGNAL_BANDWITH 125E3
#define SPREADING_FACTOR 12
#define TRANSMIT_BATTERY_VOLTAGE 1
#define CODING_RATE 5
#define SYNC_WORD 0xF3
#define PREAMBLE_LENGTH 8
#define TX_POWER 20
#define BAND 433E6  // 433E6 / 868E6 / 915E6
#define NEW_MAIL_CODE "NEWMAIL"

///////////////////////////////////////////////////////////////////////////////

int loopcounter = 0;

void setup() {
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  Serial.begin(9600);
  analogReference(VDD);
  delay(5);

  if (!LoRa.begin(BAND)) {
    Serial.println("LoRaError");
    while (1) {}
  }

  LoRa.setSignalBandwidth(SIGNAL_BANDWITH);  // signal bandwidth in Hz, defaults to 125E3
  LoRa.setSpreadingFactor(SPREADING_FACTOR);  // supports values 6 - 12, defaults to 7
  LoRa.setCodingRate4(CODING_RATE);  // supported values 5 - 8, defaults to 5
  LoRa.setSyncWord(SYNC_WORD);  // byte value to use as the sync word, defaults to 0x12
  LoRa.setPreambleLength(PREAMBLE_LENGTH);  // supports values 6 - 65535, defaults to 8
  LoRa.disableCrc();   // enable or disable CRC usage, defaults to disabled
  LoRa.setTxPower(TX_POWER);  // TX power in dB, supports values 2 - 20, defaults to 17
}


void loop() {
  if (loopcounter < 2) {
    delay(50);
    LoRa.beginPacket();
#if TRANSMIT_BATTERY_VOLTAGE
    float volts = analogReadEnh(PIN_PB4, 12) * (1.1 / 4096) * (30 + 10) / 10;
    Serial.println(volts);
    LoRa.print("{\"message\":\"" + String(NEW_MAIL_CODE) + "\",\"bat_voltage\":" + volts + "}");
#else
    LoRa.print("{\"message\":\"" + String(NEW_MAIL_CODE) + "\"}");
#endif
    LoRa.endPacket();
    delay(10);
  }

  if (loopcounter > 2) {
    LoRa.sleep();  // Put the radio in sleep mode
    digitalWrite(3, LOW);  // Sets the Latch pin 3 LOW For power cut off

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

  loopcounter++;
  delay(10);
}
