#pragma once

#include <sstream>

namespace details {
int sd_journal_print_with_location(
    int priority, const char *file, const char *line, const char *func, const char *format, ...);
}

#define vDebug(fmt, ...) \
  details::sd_journal_print_with_location(LOG_DEBUG, \
					  "CODE_FILE=" __FILE__, \
					  "CODE_LINE=" _SD_STRINGIFY(__LINE__), \
					  __func__, \
					  ##__VA_ARGS__)
#define vInfo(fmt, ...) \
  details::sd_journal_print_with_location(LOG_INFO, \
					  "CODE_FILE=" __FILE__, \
					  "CODE_LINE=" _SD_STRINGIFY(__LINE__), \
					  __func__, \
					  ##__VA_ARGS__)
#define vWarning(fmt, ...) \
  details::sd_journal_print_with_location(LOG_WARNING, \
					  "CODE_FILE=" __FILE__, \
					  "CODE_LINE=" _SD_STRINGIFY(__LINE__), \
					  __func__, \
					  ##__VA_ARGS__)
#define vError(fmt, ...) \
  details::sd_journal_print_with_location(LOG_ERR, \
					  "CODE_FILE=" __FILE__, \
					  "CODE_LINE=" _SD_STRINGIFY(__LINE__), \
					  __func__, \
					  ##__VA_ARGS__)

#define __streamLog(level, msg) \
  do { \
    std::string s = (std::stringstream() << msg).str(); \
    v##level("%s", s.data()); \
  } while (false)

#define mDebug(msg) __streamLog(Debug, msg)
#define mInfo(msg) __streamLog(Info, msg)
#define mWarning(msg) __streamLog(Warning, msg)
#define mError(msg) __streamLog(Error, msg)
