#pragma once

#include "endian.hpp"
#include "sample.hpp"

#include <chrono>
#include <vector>

namespace core
{
  enum class antenna_type : uint8_t {
    unknown,
    miniwhip,
    magnetic
  };

  struct adc_info {
    uint32_t sample_rate;
    uint16_t load_resistance;
    uint8_t bits;
    float reference_voltage;
  };

  bool operator==(const adc_info& a, const adc_info& b);

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

      static_assert(std::same_as<iterator_value_t, sample>, "expected 'sample' iterator");
      auto newsize = std::distance(begin, end);
      if (newsize >= 0) {
        _signal.resize(newsize);
        auto sbeg = _signal.begin();

        while (begin != end) {
          this->join_flags(begin->flags);
          *sbeg = to_little_endian<sample_t>(begin->data);

          ++begin;
          ++sbeg;
        }
      }
    }

    void set_flag(uint8_t bit);
    void unset_flag(uint8_t bit);
    void join_flags(uint8_t flags);

    void set_signal(data_t&& data);
    void reset_flags(uint8_t flags = 0);
    void set_antenna_type(antenna_type type);
    void set_adc_info(adc_info info);

    void swap(signal& other);

    double longitude() const;
    double latitude() const;
    double altitude() const;

    std::chrono::microseconds begin_timestamp() const;
    uint32_t sample_rate() const;
    uint8_t flags() const;
    const data_t& samples() const;
    antenna_type antenna() const;
    adc_info adc() const;

  private:
    double _lon { 0.0 };
    double _lat { 0.0 };
    double _alt { 0.0 };

    std::chrono::microseconds _begin_timestamp;
    adc_info _adc_info;

    uint8_t _flags { 0 };
    antenna_type _antenna_type { antenna_type::unknown };
    data_t _signal;  // LE data
  };
}
