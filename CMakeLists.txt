cmake_minimum_required(VERSION 3.20)

include(cmake/Version.cmake)

project(detector-core
  LANGUAGES C CXX
  VERSION ${GPIO_CORE_VERSION}
)

find_package(PkgConfig REQUIRED)

pkg_search_module(systemd REQUIRED IMPORTED_TARGET libsystemd)
pkg_search_module(sysfs REQUIRED IMPORTED_TARGET libsysfs)

add_library(detector-core OBJECT
  include/endian.hpp
  include/logging.hpp src/logging.cpp
  include/signal.hpp src/signal.cpp
  include/sample.hpp
  include/signal_file.hpp
  include/signal_file_v2.hpp src/signal_file_v2.cpp
  include/core_version.hpp src/signal_file.cpp
  include/system.hpp src/system.cpp
  src/core_version.cpp

  version
)

set(CMAKE_INCLUDE_CURRENT_DIR Yes)


target_include_directories(detector-core
  PUBLIC
    include
)

target_link_libraries(detector-core
  PUBLIC
    PkgConfig::systemd
    PkgConfig::sysfs
)

set_target_properties(detector-core
  PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED Yes
)

configure_file(detector_core_version.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/detector_core_version.hpp)
