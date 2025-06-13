#pragma once

#include <filesystem>

namespace core
{
  class signal;
}

namespace core::signal_file
{
  class signal_file_dump_error : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
  };

  class signal_file_load_error : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
  };

  enum class version {
    unknown = 0,
    v2 = 2,
    v3 = 3
  };

  inline version default_dump_version = version::v3;

  template<class Output>
  void dump(Output dst, const signal& sig, version ver = default_dump_version) = delete;

  template<class Input>
  signal load(Input src) = delete;

  template<>
  void dump<std::ostream&>(std::ostream& os, const signal& sig, version ver);

  template<>
  void dump<std::filesystem::path>(std::filesystem::path dst, const signal& sig, version ver);

  template<>
  signal load<std::istream&>(std::istream& is);

  template<>
  signal load<std::filesystem::path>(std::filesystem::path src);
}
