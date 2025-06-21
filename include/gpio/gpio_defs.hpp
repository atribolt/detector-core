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
  inline const Pin(A, 0);
  inline const Pin(A, 1);
  inline const Pin(A, 2);
  inline const Pin(A, 3);
  inline const Pin(A, 6);
  inline const Pin(A, 7);
  inline const Pin(A, 8);
  inline const Pin(A, 9);
  inline const Pin(A, 10);
  inline const Pin(A, 11);
  inline const Pin(A, 12);
  inline const Pin(A, 13);
  inline const Pin(A, 14);
  inline const Pin(A, 18);
  inline const Pin(A, 19);
  inline const Pin(A, 20);
  inline const Pin(A, 21);

  inline const Pin(C, 0);
  inline const Pin(C, 1);
  inline const Pin(C, 2);
  inline const Pin(C, 3);
  inline const Pin(C, 4);
  inline const Pin(C, 7);

  inline const Pin(G, 6);
  inline const Pin(G, 7);
  inline const Pin(G, 8);
  inline const Pin(G, 9);
#undef Pin
}
