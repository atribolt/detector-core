#pragma once

#include <cstdint>

namespace gpio
{
  enum class pin_mode {
    in = 0b000,
    out = 0b001,
    alt2 = 0b010,
    alt3 = 0b011,
    alt4 = 0b100,
    alt5 = 0b101,
    alt6 = 0b110,

    unknown = -1
  };

  enum pin_value {
    LOW,
    HIGH
  };

  struct gpio_chip {
    uint32_t* data { nullptr };
  };

  struct pin {
    const gpio_chip& chip;
    const uint32_t id;
  };
}
