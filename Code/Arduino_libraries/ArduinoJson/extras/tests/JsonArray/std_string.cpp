// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2024, Benoit BLANCHON
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

#include "Literals.hpp"

static void eraseString(std::string& str) {
  char* p = const_cast<char*>(str.c_str());
  while (*p)
    *p++ = '*';
}

TEST_CASE("std::string") {
  JsonDocument doc;
  JsonArray array = doc.to<JsonArray>();

  SECTION("add()") {
    std::string value("hello");
    array.add(value);
    eraseString(value);
    REQUIRE("hello"_s == array[0]);
  }

  SECTION("operator[]") {
    std::string value("world");
    array.add("hello");
    array[0] = value;
    eraseString(value);
    REQUIRE("world"_s == array[0]);
  }
}
