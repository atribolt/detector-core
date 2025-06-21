#pragma once

#include <string_view>

namespace gpio::board
{
  enum class id {
    orange_pi_one_h3,
    simulate,

    autodetect
  };

  std::string_view get_name();
  id detect_gpio_id();

  void initialize(id board_id = id::autodetect);
  void finalize();
}
