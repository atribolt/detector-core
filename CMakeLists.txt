cmake_minimum_required(VERSION 3.20)

project(detector LANGUAGES C CXX)

find_package(Threads REQUIRED)


add_library(detector-core OBJECT
  include/endian.hpp
  include/logging.hpp src/logging.cpp
  include/signal_file.hpp src/signal_file.cpp
  include/signal.hpp src/signal.cpp
)

set(CMAKE_INCLUDE_CURRENT_DIR Yes)


target_include_directories(detector-core
  PUBLIC
    include
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
