#include "include/signal.hpp"

namespace core
{
  void signal::set_begin_timestamp(std::chrono::microseconds ts)
  {
    _begin_timestamp = ts;
  }

  void signal::set_coords(double lon, double lat, double alt)
  {
    _lon = lon;
    _lat = lat;
    _alt = alt;
  }

  void signal::set_sample_rate(uint32_t sr)
  {
    _sample_rate = sr;
  }

  void signal::set_signal(data_t&& data)
  {
    std::swap(_signal, data);
  }

  void signal::set_flags(uint8_t flags)
  {
    _flags = flags;
  }

  void signal::swap(signal& o)
  {
    std::swap(_lon, o._lon);
    std::swap(_lat, o._lat);
    std::swap(_alt, o._alt);
    std::swap(_begin_timestamp, o._begin_timestamp);
    std::swap(_sample_rate, o._sample_rate);
    std::swap(_flags, o._flags);
    std::swap(_signal, o._signal);
  }

  double signal::longitude() const
  {
    return _lon;
  }

  double signal::latitude() const
  {
    return _lat;
  }

  double signal::altitude() const
  {
    return _alt;
  }

  std::chrono::microseconds signal::begin_timestamp() const
  {
    return _begin_timestamp;
  }

  uint32_t signal::sample_rate() const
  {
    return _sample_rate;
  }

  uint8_t signal::flags() const
  {
    return _flags;
  }

  const signal::data_t& signal::samples() const
  {
    return _signal;
  }
}
