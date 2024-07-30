// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2024, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Polyfills/type_traits.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

template <typename TIterator>
class IteratorReader {
  TIterator ptr_, end_;

 public:
  explicit IteratorReader(TIterator begin, TIterator end)
      : ptr_(begin), end_(end) {}

  int read() {
    if (ptr_ < end_)
      return static_cast<unsigned char>(*ptr_++);
    else
      return -1;
  }

  size_t readBytes(char* buffer, size_t length) {
    size_t i = 0;
    while (i < length && ptr_ < end_)
      buffer[i++] = *ptr_++;
    return i;
  }
};

template <typename TSource>
struct Reader<TSource, void_t<typename TSource::const_iterator>>
    : IteratorReader<typename TSource::const_iterator> {
  explicit Reader(const TSource& source)
      : IteratorReader<typename TSource::const_iterator>(source.begin(),
                                                         source.end()) {}
};

ARDUINOJSON_END_PRIVATE_NAMESPACE
