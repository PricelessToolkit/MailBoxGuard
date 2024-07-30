#include <UrlEncode.h>

void setup() {
  Serial.begin(115200);

  Serial.println(urlEncode("Hello, this string should be \% encoded."));
  // -> Hello%2C%20this%20string%20should%20be%20%25%20encoded.
}

void loop() {}