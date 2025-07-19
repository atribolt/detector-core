#pragma once

#include "gpio_types.hpp"

namespace gpio
{
  inline gpio_chip A;
  inline gpio_chip C;
  inline gpio_chip G;

#define Pin(chip, id) \
  pin P##chip##id     \
  {                   \
    chip, id          \
  }
  constexpr Pin(A, 0);
  constexpr Pin(A, 1);
  constexpr Pin(A, 2);
  constexpr Pin(A, 3);
  constexpr Pin(A, 6);
  constexpr Pin(A, 7);
  constexpr Pin(A, 8);
  constexpr Pin(A, 9);
  constexpr Pin(A, 10);
  constexpr Pin(A, 11);
  constexpr Pin(A, 12);
  constexpr Pin(A, 13);
  constexpr Pin(A, 14);
  constexpr Pin(A, 18);
  constexpr Pin(A, 19);
  constexpr Pin(A, 20);
  constexpr Pin(A, 21);

  constexpr Pin(C, 0);
  constexpr Pin(C, 1);
  constexpr Pin(C, 2);
  constexpr Pin(C, 3);
  constexpr Pin(C, 4);
  constexpr Pin(C, 7);

  constexpr Pin(G, 6);
  constexpr Pin(G, 7);
  constexpr Pin(G, 8);
  constexpr Pin(G, 9);
#undef Pin
}
