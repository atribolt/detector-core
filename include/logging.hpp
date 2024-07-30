#pragma once

#include <sstream>  // IWYU pragma: keep  // Code which usage logging may not to include this header for to use m<Level> macros
#include <syslog.h>

namespace core::details
{
  void log_message(int priority, const char* file, const char* line, const char* func, const char* format, ...);
}

#define _XSTRINGIFY(x) #x
#define _STRINGIFY(x) _XSTRINGIFY(x)

#define vDebug(fmt, ...)                                                                                           \
  ::core::details::log_message(LOG_DEBUG, "CODE_FILE=" __FILE__, "CODE_LINE=" _STRINGIFY(__LINE__), __func__, fmt, \
                               ##__VA_ARGS__)
#define vInfo(fmt, ...)                                                                                           \
  ::core::details::log_message(LOG_INFO, "CODE_FILE=" __FILE__, "CODE_LINE=" _STRINGIFY(__LINE__), __func__, fmt, \
                               ##__VA_ARGS__)
#define vWarning(fmt, ...)                                                                                           \
  ::core::details::log_message(LOG_WARNING, "CODE_FILE=" __FILE__, "CODE_LINE=" _STRINGIFY(__LINE__), __func__, fmt, \
                               ##__VA_ARGS__)
#define vError(fmt, ...)                                                                                         \
  ::core::details::log_message(LOG_ERR, "CODE_FILE=" __FILE__, "CODE_LINE=" _STRINGIFY(__LINE__), __func__, fmt, \
                               ##__VA_ARGS__)

#define vTrace(fmt, ...)                                                                                            \
  ::core::details::log_message(LOG_NOTICE, "CODE_FILE=" __FILE__, "CODE_LINE=" _STRINGIFY(__LINE__), __func__, fmt, \
                               ##__VA_ARGS__)

#define __streamLog(level, msg)                         \
  do {                                                  \
    std::string s = (std::stringstream() << msg).str(); \
    v##level(s.data());                                 \
  } while (false)

#define mDebug(msg) __streamLog(Debug, msg)
#define mInfo(msg) __streamLog(Info, msg)
#define mWarning(msg) __streamLog(Warning, msg)
#define mError(msg) __streamLog(Error, msg)
#define mTrace(msg) __streamLog(Trace, msg)
