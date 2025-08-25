// Youtube Channel https://www.youtube.com/@PricelessToolkit/videos
// GitHub https://github.com/PricelessToolkit/MailBoxGuard

#include <SPI.h>
#include <LoRa.h>
#include <avr/sleep.h>

///////////////////////////////// LoRa RADIO /////////////////////////////////

#define SIGNAL_BANDWITH 250E3
#define SPREADING_FACTOR 10
#define CODING_RATE 5
#define SYNC_WORD 0x12
#define PREAMBLE_LENGTH 12
#define TX_POWER 20
#define BAND 868E6 // 433E6 / 868E6 / 915E6

//////////////////////////// Name and Keys/Encryption ////////////////////////

#define NODE_NAME "mbox"
#define GATEWAY_KEY "xy" // must match CapiBridge's key
#define Encryption true                            // Global Payload obfuscation (Encryption)
#define encryption_key_length 4                    // must match number of bytes in the XOR key array
#define encryption_key { 0x4B, 0xA3, 0x3F, 0x9C }  // Multi-byte XOR key (between 2–16 values).

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

  LoRa.setSignalBandwidth(SIGNAL_BANDWITH);   // defaults to 125E3
  LoRa.setSpreadingFactor(SPREADING_FACTOR);  // 6 - 12, defaults to 7
  LoRa.setCodingRate4(CODING_RATE);           // 5 - 8, defaults to 5
  LoRa.setSyncWord(SYNC_WORD);                // defaults to 0x12
  LoRa.setPreambleLength(PREAMBLE_LENGTH);    // 6 - 65535, defaults to 8
  LoRa.disableCrc();                          // defaults to disabled
  LoRa.setTxPower(TX_POWER);                  // 2 - 20, defaults to 17
}

// -------------------- Binary-safe XOR helper -------------------- //
static inline void xorBuffer(uint8_t* buf, size_t len) {
#if Encryption
  static const uint8_t key[] = encryption_key;
  const int K = encryption_key_length;
  for (size_t i = 0; i < len; ++i) {
    buf[i] ^= key[i % K];
  }
#else
  (void)buf; (void)len;
#endif
}

void loop() {
  if (loopcounter < 2) {
    delay(10);

    // Build JSON payload
    float volts = analogReadEnh(PIN_PB4, 12) * (1.1 / 4096) * (30 + 10) / 10;
    float percentage = ((volts - 3.2) / (4.15 - 3.2)) * 100;
    percentage = constrain(percentage, 0, 100);
    int intPercentage = (int)percentage;

    String payload = "{\"k\":\"" + String(GATEWAY_KEY) + "\",\"id\":\"" + String(NODE_NAME) + "\",\"s\":\"mail\",\"b\":" + String(intPercentage) + "}";

    // Copy to buffer and XOR (binary-safe)
    uint8_t buf[255];
    size_t len = payload.length();
    if (len > sizeof(buf)) len = sizeof(buf);
    memcpy(buf, payload.c_str(), len);
    xorBuffer(buf, len);

    // Send raw bytes (not print) to preserve binary data
    LoRa.beginPacket();
    LoRa.write(buf, len);
    LoRa.endPacket();

    delay(10);
  }

  if (loopcounter > 2) {
    LoRa.sleep();             // Put the radio in sleep mode
    digitalWrite(3, LOW);     // Latch pin 3 LOW for power cut off
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_mode();             // Enter sleep mode
  }

  loopcounter++;
  delay(10);
}
