#include "include/signal_file_v1.hpp"

#include "include/endian.hpp"

#include <cmath>
#include <cstring>
#include <fstream>
#include <netinet/in.h>

namespace
{
  constexpr int kOverflowBit = 14;
  constexpr unsigned kVersionLength = 2;
  constexpr unsigned kTimeLength = 14;
  constexpr unsigned kTimeMicrosecondsLength = 4;
  constexpr unsigned kCoordsLength = 18;
  constexpr unsigned kSampleRateLength = 4;
  constexpr unsigned kSamplesCountLength = 4;
  constexpr int16_t kSampleOffset = 1000;
}  // namespace

namespace core
{
  signal_file_v1::signal_file_v1()
  {}

  signal_file_v1::signal_file_v1(signal_t&& s)
    : _signal(std::move(s))
  {}

  signal_file_v1::signal_file_v1(const signal_t& s)
    : _signal(s)
  {}

  void signal_file_v1::set_signal(signal_t&& s)
  {
    _signal.swap(s);
  }

  void signal_file_v1::set_signal(const signal_t& s)
  {
    _signal = s;
  }

  const signal_file_v1::signal_t& signal_file_v1::get_signal() const
  {
    return _signal;
  }

  std::ostream& operator<<(std::ostream& os, const signal_file_v1& s)
  {
    auto samples = s._signal.samples();
    auto begin_time = s._signal.begin_timestamp();
    auto begin_time_s = std::chrono::duration_cast<std::chrono::seconds>(begin_time);
    auto begin_time_us = (begin_time - begin_time_s);

    unsigned samplesLength = samples.size() * sizeof(decltype(samples)::value_type);
    unsigned totalSize = kVersionLength + kTimeLength + kTimeMicrosecondsLength
                       + kCoordsLength + kSampleRateLength + kSamplesCountLength;

    std::string buffer(totalSize, '\0');
    char* data = buffer.data();

    *(uint16_t*)data = to_little_endian(s.kFormatVersion);
    data += kVersionLength;

    std::string time = "yyyymmddHHMMSS\n";
    time_t ts = begin_time_s.count();
    std::strftime(time.data(), time.size(), "%Y%m%d%H%M%S", std::gmtime(&ts));
    std::copy(time.data(), time.data() + time.size(), data);
    data += kTimeLength;

    *(uint32_t*)data = to_little_endian<uint32_t>(begin_time_us.count());
    data += kTimeMicrosecondsLength;

    uint16_t lon1 = to_little_endian<uint16_t>(std::floor(s._signal.longitude()));
    uint32_t lon2 =
      to_little_endian<uint32_t>(std::floor((s._signal.longitude() - lon1) * 1'000'000));
    uint16_t lat1 = to_little_endian<uint16_t>(std::floor(s._signal.latitude()));
    uint32_t lat2 =
      to_little_endian<uint32_t>(std::floor((s._signal.latitude() - lat1) * 1'000'000));
    uint16_t alt1 = to_little_endian<uint16_t>(std::floor(s._signal.altitude()));
    uint32_t alt2 =
      to_little_endian<uint32_t>(std::floor((s._signal.altitude() - alt1) * 1'000'000));

    *(uint16_t*)data = lon1;
    data += sizeof(lon1);
    *(uint32_t*)data = lon2;
    data += sizeof(lon2);
    *(uint16_t*)data = lat1;
    data += sizeof(lat1);
    *(uint32_t*)data = lat2;
    data += sizeof(lat2);
    *(uint16_t*)data = alt1;
    data += sizeof(alt1);
    *(uint32_t*)data = alt2;
    data += sizeof(alt2);

    *(uint32_t*)data = to_little_endian<uint32_t>(s._signal.sample_rate());
    data += kSampleRateLength;

    *(uint32_t*)data = to_little_endian<uint32_t>(samples.size());
    data += kSamplesCountLength;

    os.write(buffer.data(), data - buffer.data());
    os.write((char*)samples.data(), samplesLength);
    return os;
  }
}
