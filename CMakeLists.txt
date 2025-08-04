cmake_minimum_required(VERSION 3.20)

include(cmake/Version.cmake)

project(detector-core
  LANGUAGES C CXX
  VERSION ${GPIO_CORE_VERSION}
)

set(CMAKE_CXX_STANDARD 20)
set(CAMKE_CXX_STANDARD_REQUIRED Yes)

configure_file(detector_core_version.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/detector_core_version.hpp)

find_package(PkgConfig REQUIRED)

option(CORE_JOURNALD_LOGGING "Write log to journald" No)
option(CORE_DEBUG_LOGGING "Enable debug and trace log" Yes)

option(WITH_TEST Off "Build and run test")

if (CORE_JOURNALD_LOGGING)
  pkg_search_module(systemd REQUIRED IMPORTED_TARGET libsystemd)
endif()

pkg_search_module(sysfs REQUIRED IMPORTED_TARGET libsysfs)

# ----- MsgPack -----
set(MSGPACK_CXX20 Yes CACHE BOOL "")
set(MSGPACK_USE_BOOST No CACHE BOOL "")
set(MSGPACK_BUILD_DOCS No CACHE BOOL "")
set(MSGPACK_BUILD_EXAMPLES No CACHE BOOL "")
add_subdirectory(thirdparty/msgpack EXCLUDE_FROM_ALL)
# -------------------

add_library(detector-core OBJECT
  include/endian.hpp
  include/logging.hpp
  include/signal.hpp src/signal.cpp
  include/sample.hpp
  include/signal_file.hpp
  include/gpio/gpio.hpp src/gpio/gpio.cpp
  include/gpio/gpio_defs.hpp
  include/gpio/gpio_types.hpp
  include/gpio/boards/board.hpp src/gpio/board.cpp
  include/gpio/boards/orangepi_one_h3.hpp src/gpio/orangepi_one_h3.cpp
  include/gpio/boards/simulate_gpio.hpp src/gpio/simulate_gpio.cpp
  include/utils/ring_buffer.hpp
  include/utils/ring_buffer_iterator.hpp
  include/utils/simple_vector.hpp
  include/signal_file_v2.hpp src/signal_file_v2.cpp
  include/signal_file_v3.hpp src/signal_file_v3.cpp
  include/core_version.hpp src/signal_file.cpp
  include/system.hpp src/system.cpp
  src/core_version.cpp

  ${CMAKE_CURRENT_BINARY_DIR}/detector_core_version.hpp

  version
)

set(CMAKE_INCLUDE_CURRENT_DIR Yes)


target_include_directories(detector-core
  PUBLIC
    include
)

target_compile_definitions(detector-core
  PUBLIC
    $<$<BOOL:${CORE_JOURNALD_LOGGING}>:JOURNALD_LOGGING>
    $<$<BOOL:${CORE_DEBUG_LOGGING}>:DEBUG_LOGGING>
)

target_link_libraries(detector-core
  PUBLIC
    msgpack-cxx
    $<$<BOOL:${CORE_JOURNALD_LOGGING}>:PkgConfig::systemd>
    PkgConfig::sysfs
)

set_target_properties(detector-core
  PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED Yes
)


if (WITH_TEST)
  enable_testing()

  add_executable(test_signal_v3
    tests/test_signal_v3.cpp
  )
  target_link_libraries(test_signal_v3 PRIVATE detector-core)


  add_test(
    NAME test_signal_v3
    COMMAND test_signal_v3
  )

endif()
