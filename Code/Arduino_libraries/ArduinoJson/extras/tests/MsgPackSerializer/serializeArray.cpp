// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2024, Benoit BLANCHON
// MIT License

#define ARDUINOJSON_SLOT_ID_SIZE 4  // required to reach 65536 elements

#include <ArduinoJson.h>
#include <catch.hpp>

#include "Literals.hpp"

static void check(const JsonArray array, const char* expected_data,
                  size_t expected_len) {
  std::string expected(expected_data, expected_data + expected_len);
  std::string actual;
  size_t len = serializeMsgPack(array, actual);
  CAPTURE(array);
  REQUIRE(len == expected_len);
  REQUIRE(actual == expected);
}

template <size_t N>
static void check(const JsonArray array, const char (&expected_data)[N]) {
  const size_t expected_len = N - 1;
  check(array, expected_data, expected_len);
}

static void check(const JsonArray array, const std::string& expected) {
  check(array, expected.data(), expected.length());
}

TEST_CASE("serialize MsgPack array") {
  JsonDocument doc;
  JsonArray array = doc.to<JsonArray>();

  SECTION("empty") {
    check(array, "\x90");
  }

  SECTION("fixarray") {
    array.add("hello");
    array.add("world");

    check(array, "\x92\xA5hello\xA5world");
  }

  SECTION("array 16") {
    for (int i = 0; i < 16; i++)
      array.add(i);

    check(array,
          "\xDC\x00\x10\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D"
          "\x0E\x0F");
  }

  SECTION("array 32") {
    const char* nil = 0;
    for (int i = 0; i < 65536; i++)
      array.add(nil);
    REQUIRE(array.size() == 65536);

    check(array, "\xDD\x00\x01\x00\x00"_s + std::string(65536, '\xc0'));
  }
}
