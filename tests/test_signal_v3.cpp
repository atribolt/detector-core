#include <cassert>
#include <signal.hpp>
#include <signal_file.hpp>

constexpr double Longitude = 128.012345;
constexpr double Latitude = 70.543210;
constexpr double Altitude = 150;

constexpr uint32_t SampleRate = 12'345'678;
constexpr core::antenna_type Antenna = core::antenna_type::miniwhip;

const auto Time = std::chrono::system_clock::now();
const auto TimeUs = std::chrono::duration_cast<std::chrono::microseconds>(Time.time_since_epoch());

int main()
{
  std::vector<core::sample> signal_data;
  for (int16_t i = 0; i < 100; ++i) {
    signal_data.push_back({ .data = i, .flags = 10 });
  }

  core::signal s0;
  s0.set_antenna_type(Antenna);
  s0.set_signal_data(signal_data.begin(), signal_data.end());
  s0.set_begin_timestamp(TimeUs);
  s0.set_coords(Longitude, Latitude, Altitude);
  s0.set_sample_rate(SampleRate);
  s0.set_flag(7);
  s0.set_adc_info({ .sample_rate = 500'000, .load_resistance = 1000, .resolution = 10, .reference_voltage = 3.3f });

  std::stringstream ss;
  core::signal_file::dump<std::ostream&>(ss, s0, core::signal_file::version::v3);

  core::signal s1 = core::signal_file::load<std::istream&>(ss);
  assert(s1.flags() == s0.flags());
  assert(s1.sample_rate() == s0.sample_rate());
  assert(s1.adc() == s0.adc());
  assert(s1.longitude() == s0.longitude());
  assert(s1.latitude() == s0.latitude());
  assert(s1.altitude() == s0.altitude());
  assert(s1.antenna() == s0.antenna());
  assert(s1.begin_timestamp() == s0.begin_timestamp());

  auto s0data = s0.samples();
  auto s1data = s1.samples();

  for (int i = 0; i < signal_data.size(); ++i) {
    assert(s0data[i] == s1data[i]);
  }

  return 0;
}
