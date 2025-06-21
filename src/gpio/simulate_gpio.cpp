#include "gpio/boards/simulate_gpio.hpp"

#include "gpio/gpio_defs.hpp"

#include <fcntl.h>
#include <fstream>
#include <string_view>
#include <sys/mman.h>
#include <unistd.h>

namespace
{
  constexpr std::string_view GPIO_MEM_FILE = "gpio_mem";
  uint32_t* MEM_GPIO = nullptr;

  constexpr uint32_t MODE_BLOCKS = 4;
  constexpr uint32_t DATA_BLOCKS = 1;

  constexpr uint32_t MEM_BLOCKS = (MODE_BLOCKS + DATA_BLOCKS) * 3;
  constexpr uint32_t MEM_SIZE = MEM_BLOCKS * sizeof(uint32_t);
}

void recreate_file()
{
  std::ofstream mem(GPIO_MEM_FILE.data(), std::ios::binary);
  if (!mem) {
    throw std::system_error {};
  }

  std::string buffer(MEM_SIZE, '\0');
  mem.write(buffer.c_str(), buffer.size());
  mem.close();
}

void gpio::simulate::initialize()
{
  recreate_file();

  if (MEM_GPIO == nullptr) {
    int fd = open(GPIO_MEM_FILE.data(), O_RDWR | O_SYNC | O_CLOEXEC);

    if (fd < 0) {
      throw std::system_error {};
    }

    MEM_GPIO = (uint32_t*)mmap(0, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    if (MEM_GPIO == nullptr) {
      throw std::system_error {};
    }
  }

  enum chip_def {
    a_mode = 0,
    a_data = a_mode + 4,
    c_mode = a_data + 1,
    c_data = c_mode + 4,
    g_mode = c_data + 1,
    g_data = g_mode + 4
  };

  gpio::A.data = MEM_GPIO + a_data;
  gpio::C.data = MEM_GPIO + c_data;
  gpio::G.data = MEM_GPIO + g_data;
}

void gpio::simulate::finalize()
{
  if (MEM_GPIO != nullptr) {
    munmap(MEM_GPIO, MEM_SIZE);
  }

  MEM_GPIO = nullptr;
}
