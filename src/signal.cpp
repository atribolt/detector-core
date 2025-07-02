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
    _adc_info.sample_rate = sr;
  }

  void signal::set_flag(uint8_t bit)
  {
    _flags |= 1u << bit;
  }

  void signal::unset_flag(uint8_t bit)
  {
    _flags &= ~(1u << bit);
  }

  void signal::join_flags(uint8_t flags)
  {
    _flags |= flags;
  }

  void signal::set_signal(data_t&& data)
  {
    std::swap(_signal, data);
  }

  void signal::reset_flags(uint8_t flags)
  {
    _flags = flags;
  }

  void signal::set_antenna_type(antenna_type type)
  {
    _antenna_type = type;
  }

  void signal::set_adc_info(adc_info info)
  {
    _adc_info = info;
  }

  void signal::swap(signal& o)
  {
    std::swap(_lon, o._lon);
    std::swap(_lat, o._lat);
    std::swap(_alt, o._alt);
    std::swap(_begin_timestamp, o._begin_timestamp);
    std::swap(_adc_info, o._adc_info);
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
    return _adc_info.sample_rate;
  }

  uint8_t signal::flags() const
  {
    return _flags;
  }

  antenna_type signal::antenna() const
  {
    return _antenna_type;
  }

  adc_info signal::adc() const
  {
    return _adc_info;
  }

  const signal::data_t& signal::samples() const
  {
    return _signal;
  }

  bool operator==(const adc_info& a, const adc_info& b)
  {
    return (a.load_resistance == b.load_resistance) && (a.reference_voltage == b.reference_voltage)
        && (a.bits == b.bits) && (a.sample_rate == b.sample_rate);
  }
}
