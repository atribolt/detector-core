#pragma once

#include "include/signal.hpp"

#include <cinttypes>
#include <istream>
#include <ostream>

namespace core
{
  class signal_file_v1 {
  public:
    static constexpr uint16_t kFormatVersion = 1;

    using signal_t = signal;

  public:
    signal_file_v1();
    signal_file_v1(signal_t&& s);
    signal_file_v1(const signal_t& s);

    void set_signal(signal_t&& s);
    void set_signal(const signal_t& s);

    const signal_t& get_signal() const;

  private:
    signal_t _signal;

  private:
    friend std::ostream& operator<<(std::ostream& os, const signal_file_v1& s);
  };

  std::ostream& operator<<(std::ostream& os, const signal_file_v1& s);
}
