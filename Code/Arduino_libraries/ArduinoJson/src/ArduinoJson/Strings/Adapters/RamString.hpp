// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2024, Benoit BLANCHON
// MIT License

#pragma once

#include <stddef.h>  // size_t
#include <string.h>  // strcmp

#include <ArduinoJson/Polyfills/assert.hpp>
#include <ArduinoJson/Polyfills/attributes.hpp>
#include <ArduinoJson/Strings/StringAdapter.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

template <typename T>
struct IsChar
    : integral_constant<bool, is_integral<T>::value && sizeof(T) == 1> {};

class ZeroTerminatedRamString {
 public:
  static const size_t typeSortKey = 3;

  ZeroTerminatedRamString(const char* str) : str_(str) {}

  bool isNull() const {
    return !str_;
  }

  FORCE_INLINE size_t size() const {
    return str_ ? ::strlen(str_) : 0;
  }

  char operator[](size_t i) const {
    ARDUINOJSON_ASSERT(str_ != 0);
    ARDUINOJSON_ASSERT(i <= size());
    return str_[i];
  }

  const char* data() const {
    return str_;
  }

  friend int stringCompare(ZeroTerminatedRamString a,
                           ZeroTerminatedRamString b) {
    ARDUINOJSON_ASSERT(!a.isNull());
    ARDUINOJSON_ASSERT(!b.isNull());
    return ::strcmp(a.str_, b.str_);
  }

  friend bool stringEquals(ZeroTerminatedRamString a,
                           ZeroTerminatedRamString b) {
    return stringCompare(a, b) == 0;
  }

  bool isLinked() const {
    return false;
  }

 protected:
  const char* str_;
};

template <typename TChar>
struct StringAdapter<TChar*, enable_if_t<IsChar<TChar>::value>> {
  typedef ZeroTerminatedRamString AdaptedString;

  static AdaptedString adapt(const TChar* p) {
    return AdaptedString(reinterpret_cast<const char*>(p));
  }
};

template <typename TChar, size_t N>
struct StringAdapter<TChar[N], enable_if_t<IsChar<TChar>::value>> {
  typedef ZeroTerminatedRamString AdaptedString;

  static AdaptedString adapt(const TChar* p) {
    return AdaptedString(reinterpret_cast<const char*>(p));
  }
};

class StaticStringAdapter : public ZeroTerminatedRamString {
 public:
  StaticStringAdapter(const char* str) : ZeroTerminatedRamString(str) {}

  bool isLinked() const {
    return true;
  }
};

template <>
struct StringAdapter<const char*, void> {
  typedef StaticStringAdapter AdaptedString;

  static AdaptedString adapt(const char* p) {
    return AdaptedString(p);
  }
};

class SizedRamString {
 public:
  static const size_t typeSortKey = 2;

  SizedRamString(const char* str, size_t sz) : str_(str), size_(sz) {}

  bool isNull() const {
    return !str_;
  }

  size_t size() const {
    return size_;
  }

  char operator[](size_t i) const {
    ARDUINOJSON_ASSERT(str_ != 0);
    ARDUINOJSON_ASSERT(i <= size());
    return str_[i];
  }

  const char* data() const {
    return str_;
  }

  bool isLinked() const {
    return false;
  }

 protected:
  const char* str_;
  size_t size_;
};

template <typename TChar>
struct SizedStringAdapter<TChar*, enable_if_t<IsChar<TChar>::value>> {
  typedef SizedRamString AdaptedString;

  static AdaptedString adapt(const TChar* p, size_t n) {
    return AdaptedString(reinterpret_cast<const char*>(p), n);
  }
};

ARDUINOJSON_END_PRIVATE_NAMESPACE
