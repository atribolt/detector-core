#pragma once

#include "signal_file.hpp"

#include <istream>
#include <ostream>

/**
 * V3 signal has msgpack format
 */

namespace core::signal_file::v3
{
  void dump(const signal& sig, std::ostream& os);
  signal load(std::istream& from);
}
