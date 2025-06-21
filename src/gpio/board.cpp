#include "gpio/boards/board.hpp"

#include "gpio/boards/orangepi_one_h3.hpp"
#include "gpio/boards/simulate_gpio.hpp"
#include "logging.hpp"

#include <fstream>
#include <regex>

namespace
{
  void (*finalizer)() = nullptr;
}

std::string_view gpio::board::get_name()
{
  static std::string name;

  if (name.empty()) {
    const std::regex board_name_pattern { R"(^BOARD=(.+?)$)" };
    std::ifstream file("/etc/armbian-release");

    std::string line;
    while (file && !std::getline(file, line).eof()) {
      std::smatch res;
      if (std::regex_match(line, res, board_name_pattern)) {
        name = res.str(1);
      }
    }

    if (name.empty()) {
      name = "simulate";
    }

    file.close();
  }

  return name;
}

gpio::board::id gpio::board::detect_gpio_id()
{
  if (get_name() == "orangepione") {
    return id::orange_pi_one_h3;
  }

  return id::simulate;
}

void gpio::board::initialize(id board_id)
{
  if (finalizer) {
    vWarning("it is initialized");
    return;
  }

  if (board_id == id::autodetect) {
    board_id = detect_gpio_id();
  }

  switch (board_id) {
    case id::orange_pi_one_h3:
      h3::initialize();
      finalizer = h3::finalize;
      break;

    default:
      simulate::initialize();
      finalizer = simulate::finalize;
      break;
  }

  vDebug("%s GPIO initialized", get_name().data());
}

void gpio::board::finalize()
{
  if (finalizer) {
    finalizer();
    vDebug("%s GPIO finalized", get_name().data());
  }

  finalizer = nullptr;
}
