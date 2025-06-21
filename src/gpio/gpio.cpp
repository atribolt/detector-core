#include "gpio/gpio.hpp"

#include "gpio/boards/board.hpp"
#include "logging.hpp"

namespace
{
  struct initializer {
    initializer(gpio::board::id id)
    {
      gpio::board::initialize(id);
    }

    ~initializer()
    {
      gpio::board::finalize();
    }
  };

  constexpr uint32_t MODE_MASK = 0b111;
}

gpio::pin_mode gpio::get_mode(pin p)
{
  uint8_t index = ((p.id & ~MODE_MASK) >> 3);
  uint32_t bank = (p.chip.data - 4)[index];
  uint8_t offset = (p.id & MODE_MASK) << 2;

  return pin_mode((bank >> offset) & MODE_MASK);
}

void gpio::set_mode(pin p, pin_mode value)
{
  uint32_t index = (p.id & (~MODE_MASK)) >> 3;
  uint32_t* bank = p.chip.data - 4 + index;
  uint32_t offset = (p.id & MODE_MASK) << 2;

  uint32_t mode = uint32_t(value) << offset;
  uint32_t old_mode = ((*bank) >> offset) & MODE_MASK;

  uint32_t zero_old_mode = (*bank) & ~(MODE_MASK << offset);

  (*bank) = zero_old_mode | mode;

  vDebug("Pin %d mode changed: %X -> %X", p.id, old_mode, uint32_t(value));
  vDebug("Pin %d i: %d, o: %d", p.id, index, offset);
}

gpio::pin_value gpio::get_pin_value(pin p)
{
  return pin_value(((*p.chip.data) >> p.id) & 1u);
}

void gpio::set_pin_value(pin p, pin_value v)
{
  switch (v) {
    case pin_value::LOW:
      (*p.chip.data) &= ~(1 << p.id);
      break;
    case pin_value::HIGH:
      (*p.chip.data) |= (1 << p.id);
      break;
  }
}

uint32_t gpio::get_raw_values(gpio_chip chip)
{
  return *chip.data;
}

void gpio::initialize()
{
  static initializer _ { board::id::autodetect };
}
