#pragma once

#include <cstdint>


namespace core
{
struct sample
{
  enum flag {
    overflow = 0b001,
    pps_sync = 0b010
  };

  int16_t data {0};
  uint8_t flags {0};
};
}
