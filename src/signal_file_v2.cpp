#include "include/signal_file_v2.hpp"

#include "include/endian.hpp"
#include "include/signal.hpp"

#include <cmath>
#include <cstring>

namespace
{
  constexpr unsigned kVersionLength = 2;
  constexpr unsigned kTimeLength = 14;
  constexpr unsigned kTimeMicrosecondsLength = 4;
  constexpr unsigned kCoordsLength = 18;
  constexpr unsigned kSampleRateLength = 4;
  constexpr unsigned kFlagsLength = 1;
  constexpr unsigned kSamplesCountLength = 4;

  constexpr unsigned kMaxSize = std::max({ kVersionLength, kTimeLength, kTimeMicrosecondsLength, kCoordsLength,
                                           kSampleRateLength, kFlagsLength, kSamplesCountLength });

  constexpr std::string_view kTimeFormat = "%Y%m%d%H%M%S";
}

void core::signal_file::v2::dump(const signal& sig, std::ostream& to)
{
  auto samples = sig.samples();
  auto begin_time = sig.begin_timestamp();
  auto begin_time_s = std::chrono::duration_cast<std::chrono::seconds>(begin_time);
  auto begin_time_us = (begin_time - begin_time_s);

  unsigned samplesLength = samples.size() * sizeof(decltype(samples)::value_type);
  unsigned totalSize =
    kTimeLength + kTimeMicrosecondsLength + kCoordsLength + kSampleRateLength + kFlagsLength + kSamplesCountLength;

  std::string buffer(totalSize, '\0');
  char* data = buffer.data();

  std::string time = "yyyymmddHHMMSS\n";
  time_t ts = begin_time_s.count();
  std::strftime(time.data(), time.size(), kTimeFormat.data(), std::gmtime(&ts));
  std::copy(time.data(), time.data() + time.size(), data);
  data += kTimeLength;

  *(uint32_t*)data = to_little_endian<uint32_t>(begin_time_us.count());
  data += kTimeMicrosecondsLength;

  uint16_t lon1 = to_little_endian<uint16_t>(std::floor(sig.longitude()));
  uint32_t lon2 = to_little_endian<uint32_t>(std::floor((sig.longitude() - lon1) * 1'000'000));
  uint16_t lat1 = to_little_endian<uint16_t>(std::floor(sig.latitude()));
  uint32_t lat2 = to_little_endian<uint32_t>(std::floor((sig.latitude() - lat1) * 1'000'000));
  uint16_t alt1 = to_little_endian<uint16_t>(std::floor(sig.altitude()));
  uint32_t alt2 = to_little_endian<uint32_t>(std::floor((sig.altitude() - alt1) * 1'000'000));

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

  *(uint32_t*)data = to_little_endian<uint32_t>(sig.sample_rate());
  data += kSampleRateLength;

  *(uint8_t*)data = sig.flags();
  data += kFlagsLength;

  *(uint32_t*)data = to_little_endian<uint32_t>(samples.size());
  data += kSamplesCountLength;

  to.write(buffer.data(), data - buffer.data());
  to.write((char*)samples.data(), samplesLength);
}

core::signal core::signal_file::v2::load(std::istream& from)
{
  signal result;

  char buffer[kMaxSize] = { 0 };

  {  // load time
    tm time;
    memset(&time, 0, sizeof(tm));
    from >> std::get_time(&time, kTimeFormat.data());

    if (from.bad()) {
      throw signal_file_load_error { "error while read time" };
    }

    time_t begin_tm_s = timegm(&time);
    std::chrono::microseconds begin_time = std::chrono::seconds { begin_tm_s };

    from.read(buffer, kTimeMicrosecondsLength);
    if (from.gcount() != kTimeMicrosecondsLength) {
      throw signal_file_load_error { "error while read time microseconds" };
    }

    uint32_t begin_time_us = from_little_endian<uint32_t>(buffer);
    begin_time += std::chrono::microseconds(begin_time_us);
    result.set_begin_timestamp(begin_time);
  }

  {  // load coords
    from.read(buffer, kCoordsLength);
    if (from.gcount() != kCoordsLength) {
      throw signal_file_load_error { "error while read coords" };
    }

    uint16_t lon1 = from_little_endian<uint16_t>(buffer);
    uint32_t lon2 = from_little_endian<uint32_t>(buffer + 2);
    uint16_t lat1 = from_little_endian<uint16_t>(buffer + 6);
    uint32_t lat2 = from_little_endian<uint32_t>(buffer + 8);
    uint16_t alt1 = from_little_endian<uint16_t>(buffer + 12);
    uint32_t alt2 = from_little_endian<uint16_t>(buffer + 14);

    double lon = double(lon1) + double(lon2) / 1'000'000;
    double lat = double(lat1) + double(lat2) / 1'000'000;
    double alt = double(alt1) + double(alt2) / 1'000'000;

    result.set_coords(lon, lat, alt);
  }

  {  // load sample rate
    from.read(buffer, kSampleRateLength);
    if (from.gcount() != kSampleRateLength) {
      throw signal_file_load_error { "error while read sample rate" };
    }

    uint32_t sample_rate = from_little_endian<uint32_t>(buffer);
    result.set_sample_rate(sample_rate);
  }

  {  // load flags
    from.read(buffer, kFlagsLength);
    if (from.gcount() != kFlagsLength) {
      throw signal_file_load_error { "error while read flags" };
    }

    uint8_t flags = from_little_endian<uint8_t>(buffer);
    result.reset_flags(flags);
  }

  {  // load samples
    from.read(buffer, kSamplesCountLength);
    if (from.gcount() != kSamplesCountLength) {
      throw signal_file_load_error { "error while read samples count" };
    }

    uint32_t count = from_little_endian<uint32_t>(buffer);
    signal::data_t samples;
    samples.reserve(count);

    uint32_t countBytes = count * sizeof(signal::data_t::value_type);
    from.read((char*)samples.data(), countBytes);
    if (from.gcount() != countBytes) {
      throw signal_file_load_error { "error while read samples" };
    }

    result.set_signal(std::move(samples));
  }

  return result;
}
