#include "include/logging.hpp"

#include <cstdarg>

#ifdef NDEBUG
#  include <systemd/sd-journal.h>
#else
#  include <ctime>
#  include <iostream>
#  include <syncstream>
#endif

void core::details::log_message(int priority, const char* file, const char* line, const char* func, const char* format,
                                ...)
{
#ifdef NDEBUG
  va_list list;
  va_start(list, format);
  sd_journal_printv_with_location(priority, file, line, func, format, list);
  va_end(list);
#else
  // костыль для отладки

  std::timespec time;
  clock_gettime(CLOCK_MONOTONIC, &time);

  va_list list;
  va_start(list, format);
  int required = vsnprintf(nullptr, 0, format, list);
  va_end(list);

  std::string buffer(required, '\0');

  va_start(list, format);
  vsnprintf(buffer.data(), buffer.size(), format, list);
  va_end(list);

  std::osyncstream { std::cout } << time.tv_sec << '.' << time.tv_nsec << ": [" << priority << "] {" << func << ":"
                                 << line << "} " << buffer << '\n';
#endif
}
