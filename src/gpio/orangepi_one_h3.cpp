#include "gpio/boards/orangepi_one_h3.hpp"

#include "gpio/gpio_defs.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <system_error>
#include <unistd.h>

namespace
{
  uint32_t* MEM_CCU = nullptr;

  constexpr uint32_t MAP_CCU = 0x01'C2'00'00;
  constexpr uint32_t MAP_PIO = 0x01'C2'08'00;
  constexpr uint32_t PIO_OFFSET = (MAP_PIO - MAP_CCU);

  constexpr uint32_t CCU_SIZE = 1024;
  constexpr uint32_t PIO_SIZE = 1024;
  constexpr uint32_t ALL_SIZE = CCU_SIZE + PIO_SIZE;
}

void gpio::h3::initialize()
{
  if (MEM_CCU == nullptr) {
    int fd = open("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC);

    if (fd < 0) {
      throw std::system_error {};
    }

    MEM_CCU = (uint32_t*)mmap(0, ALL_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, MAP_CCU);
    close(fd);

    if (MEM_CCU == nullptr) {
      throw std::system_error {};
    }
  }

  gpio::A.data = MEM_CCU + ((PIO_OFFSET + 0x10) >> 2);
  gpio::C.data = MEM_CCU + ((PIO_OFFSET + 0x58) >> 2);
  gpio::G.data = MEM_CCU + ((PIO_OFFSET + 0xe8) >> 2);
}

void gpio::h3::finalize()
{
  if (MEM_CCU != nullptr) {
    munmap(MEM_CCU, ALL_SIZE);
  }

  MEM_CCU = nullptr;
}
