#pragma once

#include "signal_file.hpp"

#include <cinttypes>
#include <istream>
#include <ostream>

namespace core::signal_file::v2
{
  void dump(const signal& sig, std::ostream& to);
  signal load(std::istream& from);
}
