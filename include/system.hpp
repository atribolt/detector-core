#pragma once

#include <string>

namespace core::system
{
  struct info {
    [[deprecated("Use 'cpu_serial' instead")]]
    std::string motherboard_id;

    std::string cpu_serial;
    std::string mac_address;
  };

  info get_system_info();

}  // namespace core::system
