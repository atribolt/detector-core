#include "include/signal_file_v3.hpp"

#include "include/endian.hpp"
#include "include/signal.hpp"

#include <msgpack.hpp>

#include <cmath>
#include <cstring>

namespace
{
  constexpr std::string_view kTimeFormat = "%Y%m%d%H%M%S";
  constexpr std::string_view kTimeDefault = "yyyymmddHHMMSS\n";
}


void core::signal_file::v3::dump(const signal& sig, std::ostream& to)
{
  auto samples = sig.samples();
  auto begin_time = sig.begin_timestamp();
  auto begin_time_s = std::chrono::duration_cast<std::chrono::seconds>(begin_time);
  auto begin_time_us = (begin_time - begin_time_s);

  std::string time { kTimeDefault };
  time_t ts = begin_time_s.count();
  std::strftime(time.data(), time.size(), kTimeFormat.data(), std::gmtime(&ts));

  msgpack::object d;

  msgpack::pack(to, time);
  msgpack::pack(to, begin_time_us.count());

  // TODO упаковать остальные данные
}

core::signal core::signal_file::v3::load(std::istream& from)
{
  // TODO добавить распаковку сигнала из потока
}
