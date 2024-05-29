#pragma once

#include <string>

namespace core::system
{
  struct info {
    std::string motherboard_id;
    std::string mac_address;
  };

  info get_system_info();

}  // namespace core::system
