#pragma once

#include <string>

namespace core::system
{
  struct info {
    std::string cpu_serial;
    std::string mac_address;
  };

  info get_system_info();

}  // namespace core::system
