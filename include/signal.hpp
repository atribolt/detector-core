#pragma once

#include "sample.hpp"

#include <chrono>
#include <cinttypes>
#include <vector>

namespace core
{
  class signal {
  public:
    using sample_t = decltype(sample::data);
    using data_t = std::vector<sample_t>;

  public:
    //! @param ts UTC timestamp from start since epoch
    void set_begin_timestamp(std::chrono::microseconds ts);
    void set_coords(double lon, double lat, double alt);
    void set_sample_rate(uint32_t sr);

    template<class It>
    void set_signal_data(It begin, It end)
    {
      using iterator_value_t = typename std::iterator_traits<It>::value_type;
      static_assert(std::is_same_v<iterator_value_t, sample>,
                    "Expected `sample` iterator");

      auto newsize = std::distance(begin, end);
      if (newsize >= 0) {
        _signal.resize(newsize);
        auto sbeg = _signal.begin();

        while (begin != end) {
          _flags |= begin->flags;
          *sbeg = to_little_endian<sample_t>(begin->data);

          ++begin;
          ++sbeg;
        }
      }
    }

    void swap(signal& other);

    double longitude() const;
    double latitude() const;
    double altitude() const;

    std::chrono::microseconds begin_timestamp() const;
    uint32_t sample_rate() const;
    uint8_t flags() const;
    const data_t& samples() const;

  private:
    double _lon { 0.0 };
    double _lat { 0.0 };
    double _alt { 0.0 };

    std::chrono::microseconds _begin_timestamp;
    uint32_t _sample_rate { 1 };

    uint8_t _flags { 0 };
    data_t _signal;  // LE data
  };
}
