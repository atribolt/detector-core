#include "include/signal_file_v3.hpp"

#include "include/signal.hpp"

#include <msgpack.hpp>

namespace
{
  constexpr std::string_view kTimeFormat = "%Y%m%d%H%M%S";
  constexpr std::string_view kTimeDefault = "yyyymmddHHMMSS\n";
}

void core::signal_file::v3::dump(const signal& sig, std::ostream& os)
{
  auto samples = sig.samples();
  auto begin_time = sig.begin_timestamp();
  auto begin_time_s = std::chrono::duration_cast<std::chrono::seconds>(begin_time);
  auto begin_time_us = (begin_time - begin_time_s);

  std::string time { kTimeDefault };
  time_t ts = begin_time_s.count();
  std::strftime(time.data(), time.size(), kTimeFormat.data(), std::gmtime(&ts));
  time.pop_back();

  msgpack::pack(os, time);
  msgpack::pack(os, begin_time_us.count());
  msgpack::pack(os, sig.latitude());
  msgpack::pack(os, sig.longitude());
  msgpack::pack(os, sig.altitude());
  msgpack::pack(os, uint32_t(sig.sample_rate()));
  msgpack::pack(os, uint16_t(sig.adc().load_resistance));
  msgpack::pack(os, uint8_t(sig.adc().bits));
  msgpack::pack(os, double(sig.adc().reference_voltage));
  msgpack::pack(os, uint8_t(sig.flags()));
  msgpack::pack(os, uint8_t(sig.antenna()));
  msgpack::pack(os, sig.samples());
}

static core::signal load_v3(std::istream& is, bool is_v31) {
  using namespace core;
  using namespace core::signal_file;

  enum class field {
    date_time,
    microseconds,
    latitude,
    longitude,
    altitude,
    sample_rate,
    adc_load_resistance,
    adc_bits,
    adc_reference_voltage,
    flags,
    antenna,
    signal,
    threshold,
    completed
  };

  constexpr size_t BufferSize = 4096;
  msgpack::unpacker u;
  u.reserve_buffer(BufferSize);

  std::chrono::microseconds begin_time;
  double lon, lat, alt;
  uint8_t flags;
  antenna_type antenna;
  signal::data_t samples;
  adc_info adc;
  int16_t threshold = 0;

  field process = field::date_time;
  while (process != field::completed) {
    is.read(u.buffer(), BufferSize);

    if (is.gcount() <= 0) {
      throw signal_file_load_error { "invalid input stream" };
    }

    u.buffer_consumed(is.gcount());
    msgpack::object_handle obj;
    while (u.next(obj)) {
      switch (process) {
      case field::date_time: {
        std::string time_str = obj->as<std::string>();
        tm time;
        memset(&time, 0, sizeof(tm));
        strptime(time_str.data(), kTimeFormat.data(), &time);
        if (errno) {
          throw signal_file_load_error { "error while read time microseconds" };
        }
        begin_time = std::chrono::seconds(timegm(&time));

        process = field::microseconds;
        break;
      }

      case field::microseconds: {
        uint32_t us = obj->as<uint32_t>();
        begin_time += std::chrono::microseconds(us);

        process = field::latitude;
        break;
      }

      case field::longitude: {
        obj->convert(lon);
        process = field::altitude;
        break;
      }
      case field::latitude: {
        obj->convert(lat);
        process = field::longitude;
        break;
      }
      case field::altitude: {
        obj->convert(alt);
        process = field::sample_rate;
        break;
      }
      case field::sample_rate: {
        obj->convert(adc.sample_rate);
        process = field::adc_load_resistance;
        break;
      }
      case field::adc_load_resistance: {
        obj->convert(adc.load_resistance);
        process = field::adc_bits;
        break;
      }
      case field::adc_bits: {
        obj->convert(adc.bits);
        process = field::adc_reference_voltage;
        break;
      }
      case field::adc_reference_voltage: {
        obj->convert(adc.reference_voltage);
        process = field::flags;
        break;
      }
      case field::flags: {
        obj->convert(flags);
        process = field::antenna;
        break;
      }
      case field::antenna: {
        antenna = antenna_type(obj->as<uint8_t>());
        switch (antenna) {
        case antenna_type::unknown:
        case antenna_type::miniwhip:
        case antenna_type::magnetic:
          break;

        default:
          throw signal_file_load_error { "invalid antenna type" };
        }

        process = field::signal;
        break;
      }
      case field::signal: {
        obj->convert(samples);
        process = is_v31 ? field::threshold : field::completed;
        break;
      }
      case field::threshold: {
        obj->convert(threshold);
        process = field::completed;
        break;
      }
      case field::completed:
        break;
      }
    }
  }

  signal sig;
  sig.set_begin_timestamp(begin_time);
  sig.set_coords(lon, lat, alt);
  sig.set_adc_info(adc);
  sig.reset_flags(flags);
  sig.set_antenna_type(antenna);
  sig.set_signal(std::move(samples));
  sig.set_threshold(threshold);
  return sig;
}

core::signal core::signal_file::v3::load(std::istream& is)
{
  return load_v3(is, false);
}

void core::signal_file::v31::dump(const signal &sig, std::ostream &os)
{
  core::signal_file::v3::dump(sig, os);
  msgpack::pack(os, sig.threshold());
}

core::signal core::signal_file::v31::load(std::istream &from)
{
  return load_v3(from, true);
}
