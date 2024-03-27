#include "include/signal_file.hpp"

#include "include/endian.hpp"
#include "include/logging.hpp"
#include "include/signal.hpp"
#include "include/signal_file_v2.hpp"

#include <fstream>

namespace core::signal_file
{
  template<>
  void dump<std::filesystem::path>(std::filesystem::path dst, const signal& sig, version ver)
  {
    std::ofstream out { dst, std::ios::binary };
    if (!out) {
      throw signal_file_dump_error { "error while open file for dump" };
    }

    dump<std::ostream&>(out, sig, ver);

    out.flush();
    out.close();
  }

  template<>
  void dump<std::ostream&>(std::ostream& os, const signal& sig, version ver)
  {
    void (*dump_fn)(const signal&, std::ostream&) = nullptr;

    switch (ver) {
      case version::v2:
        dump_fn = v2::dump;
        break;
      default:
        throw signal_file_dump_error { "invalid version for dump" };
    }

    dump_fn(sig, os);
  }

  template<>
  signal load<std::istream&>(std::istream& is)
  {
    char buffer[2] = { 0 };
    is.read(buffer, 2);

    version ver = version(from_little_endian<uint16_t>(buffer));

    signal (*load_fn)(std::istream&) = nullptr;

    switch (ver) {
      case version::v2:
        load_fn = v2::load;
        break;
      default:
        throw signal_file_load_error { "invalid version for load" };
    }

    return load_fn(is);
  }

  template<>
  signal load<std::filesystem::path>(std::filesystem::path src)
  {
    std::ifstream in { src, std::ios::binary };
    if (!in) {
      throw signal_file_load_error { "error while open file for load" };
    }

    signal sig = load<std::istream&>(in);
    in.close();
    return sig;
  }
}
