#pragma once

#include "gpio_types.hpp"

namespace gpio
{
  void initialize();

  pin_mode get_mode(pin p);
  void set_mode(pin p, pin_mode mode);

  pin_value get_pin_value(pin p);
  void set_pin_value(pin p, pin_value v);

  uint32_t get_raw_values(gpio_chip chip);

#define GET_PIN_VALUE_FROM_RAW(r, p) (((r) >> (p).id) & 1u)
}
