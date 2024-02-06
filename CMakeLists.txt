cmake_minimum_required(VERSION 3.20)

set(CORE_VERSION 0.0.1)

project(detector-core
  LANGUAGES C CXX
  VERSION ${CORE_VERSION}
)

find_package(Threads REQUIRED)


add_library(detector-core OBJECT
  include/endian.hpp
  include/logging.hpp src/logging.cpp
  include/signal.hpp src/signal.cpp
  include/sample.hpp
  include/signal_file_v2.hpp src/signal_file_v2.cpp
)

set(CMAKE_INCLUDE_CURRENT_DIR Yes)


target_include_directories(detector-core
  PUBLIC
    include
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(detector-core
  PUBLIC
    systemd
)

set_target_properties(detector-core
  PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED Yes
)

configure_file(detector_core_version.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/detector_core_version.hpp)
