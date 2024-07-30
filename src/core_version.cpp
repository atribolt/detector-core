#include "core_version.hpp"

#include "detector_core_version.hpp"

std::string_view core::version()
{
  return DETECTOR_CORE_VERSION " " DETECTOR_CORE_COMMIT;
}
