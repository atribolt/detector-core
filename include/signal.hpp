#pragma once

#include <chrono>
#include <cinttypes>
#include <vector>

namespace core
{
  class signal {
  public:
    using sample_t = int16_t;
    using data_t = std::vector<sample_t>;

  public:
    //! @param ts UTC timestamp from start since epoch
    void set_begin_timestamp(std::chrono::microseconds ts);
    void set_coords(double lon, double lat, double alt);
    void set_sample_rate(uint32_t sr);
    void set_signal_data(const data_t& signal);

    void swap(signal& other);

    double longitude() const;
    double latitude() const;
    double altitude() const;

    std::chrono::microseconds begin_timestamp() const;
    uint32_t sample_rate() const;
    const data_t& samples() const;

  private:
    double _lon { 0.0 };
    double _lat { 0.0 };
    double _alt { 0.0 };

    std::chrono::microseconds _begin_timestamp;
    uint32_t _sample_rate { 1 };

    data_t _signal;  // byte order LE
  };
}
