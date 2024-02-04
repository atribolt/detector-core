#include "include/logging.hpp"

#include <systemd/sd-journal.h>

int details::sd_journal_print_with_location(int priority, const char* file,
                                            const char* line, const char* func,
                                            const char* format, ...)
{
  va_list list;
  va_start(list, format);
  int status = sd_journal_printv_with_location(priority, file, line, func, format, list);
  va_end(list);

  return status;
}
