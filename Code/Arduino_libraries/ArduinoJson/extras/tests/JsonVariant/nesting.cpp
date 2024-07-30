// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2024, Benoit BLANCHON
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonVariant::nesting()") {
  JsonDocument doc;
  JsonVariant var = doc.to<JsonVariant>();

  SECTION("return 0 if uninitialized") {
    JsonVariant unitialized;
    REQUIRE(unitialized.nesting() == 0);
  }

  SECTION("returns 0 for string") {
    var.set("hello");
    REQUIRE(var.nesting() == 0);
  }

  SECTION("returns 1 for empty object") {
    var.to<JsonObject>();
    REQUIRE(var.nesting() == 1);
  }

  SECTION("returns 1 for empty array") {
    var.to<JsonArray>();
    REQUIRE(var.nesting() == 1);
  }
}
