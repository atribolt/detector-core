#pragma once

#include <bit>
#include <algorithm>


template<class T>
  T to_little_endian(T num) {
    constexpr auto size = sizeof(T);

    if constexpr (size > 1 && std::endian::native == std::endian::big) {
      std::reverse((char*)&num, (char*)&num + size);
    }

    return num;
  }

template<class T>
  T from_little_endian(T num) {
    constexpr auto size = sizeof(T);

    if constexpr (size > 1 && std::endian::native == std::endian::big) {
      std::reverse((char*)&num, (char*)&num + size);
    }

    return num;
  }

template<class Tout>
  Tout from_little_endian(const void* data) {
    return from_little_endian<Tout>(
      *reinterpret_cast<const Tout*>(data)
    );
  }
