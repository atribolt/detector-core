#pragma once

#include "gpio_types.hpp"

#define GET_BIT(r, b) (((r) >> (b)) & 1u)

namespace gpio
{
  void initialize();

  pin_mode get_mode(pin p);
  void set_mode(pin p, pin_mode mode);

  pin_value get_pin_value(pin p);
  void set_pin_value(pin p, pin_value v);

  uint32_t get_raw_values(gpio_chip chip);

  template<gpio::pin P0, gpio::pin... PN>
  inline int16_t unpack_sample(uint32_t raw)
  {
    static_assert((sizeof...(PN) + 1) < 16, "too many pins, 16 limit");
    if constexpr (sizeof...(PN)) {
      return (GET_BIT(raw, P0.id) << sizeof...(PN)) | unpack_sample<PN...>(raw);
    }
    else {
      return GET_BIT(raw, P0.id);
    }
  }

  template<gpio::pin... PN>
  struct pin_order {
    static int16_t unpack(uint32_t raw)
    {
      return unpack_sample<PN...>(raw);
    }

    static void mode(pin_mode mode)
    {
      (gpio::set_mode(PN, mode), ...);
    }
  };

  template<gpio::pin PN>
  pin_value get_pin_value(pin_order<PN>)
  {
    return get_pin_value(PN);
  }
}
