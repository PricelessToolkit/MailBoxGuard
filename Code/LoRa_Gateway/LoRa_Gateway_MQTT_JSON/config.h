/////////////////////////// LoRa Gateway Key ///////////////////////////

#define GATEWAY_KEY "xy" // Keep it Tiny

///////////////////////////////////////////////////////////////////////////////

////////////////////////////// WIFI / MQTT ////////////////////////////////////

#define WIFI_SSID "xxxx"
#define WIFI_PASSWORD "xxxx"
#define MQTT_USERNAME "xxxx"
#define MQTT_PASSWORD "xxxx"
#define MQTT_SERVER "xxxx"
#define MQTT_PORT 1883

///////////////////////////////////////////////////////////////////////////////


////////////////////////////// LORA CONFIG ////////////////////////////////////

// For sending 6 bytes "6 characters" of data using different Spreading Factors (SF), the estimated time on air (ToA)
// for each SF is as follows:

// SF7: Approximately 0.027 seconds (27.494 milliseconds)
// SF8: Approximately 0.052 seconds (52.224 milliseconds)
// SF9: Approximately 0.100 seconds (100.147 milliseconds)
// SF10: Approximately 0.193 seconds (193.413 milliseconds)
// SF11: Approximately 0.385 seconds (385.297 milliseconds)
// SF12: Approximately 0.746 seconds (746.127 milliseconds)
// These calculations demonstrate how the time on air increases with higher Spreading Factors
// due to the decreased data rate, which is a trade-off for increased communication range and signal robustness.

#define SIGNAL_BANDWITH 125E3  // signal bandwidth in Hz, defaults to 125E3
#define SPREADING_FACTOR 8    // ranges from 6-12,default 7 see API docs
#define CODING_RATE 5          // Supported values are between 5 and 8, these correspond to coding rates of 4/5 and 4/8. The coding rate numerator is fixed at 4.
#define SYNC_WORD 0xF3         // byte value to use as the sync word, defaults to 0x12
#define PREAMBLE_LENGTH 6      // Supported values are between 6 and 65535.
#define TX_POWER 20            // TX power in dB, defaults to 17, Supported values are 2 to 20
#define BAND 868E6             // 433E6 / 868E6 / 915E6

///////////////////////////////////////////////////////////////////////////////
