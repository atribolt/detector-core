#pragma once

#include <sstream>  // IWYU pragma: keep  // Code which usage logging may not to include this header for to use m<Level> macros
#include <syslog.h>


#ifdef JOURNALD_LOGGING

#define _XSTRINGIFY(x) #x
#define _STRINGIFY(x) _XSTRINGIFY(x)

#include <systemd/sd-journal.h>

#define JOURNALD_FILE_PREFIX "CODE_FILE="
#define JOURNALD_LINE_PREFIX "CODE_LINE="

#define vInfo(fmt, ...)                                                                                           \
  sd_journal_print(LOG_INFO, fmt, ##__VA_ARGS__)
#define vWarning(fmt, ...)                                                                                           \
  sd_journal_print(LOG_WARNING, fmt, ##__VA_ARGS__)
#define vError(fmt, ...)                                                                                         \
  sd_journal_print(LOG_ERR, fmt, ##__VA_ARGS__)

#ifdef DEBUG_LOGGING
#define vDebug(fmt, ...)                                                                                           \
  sd_journal_print(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define vTrace(fmt, ...)                                                                                            \
  sd_journal_print(LOG_NOTICE, fmt, ##__VA_ARGS__)
#else
#define vDebug(fmt, ...)
#define vTrace(fmt, ...)
#endif

#else

#include <time.h>

namespace core::details
{
#ifdef DEBUG_LOGGING
  constexpr bool kLogDebug = true;
#else
  constexpr bool kLogDebug = false;
#endif

  template<int level>
  struct level_trait
  {
    static int enable;
  };

  template<>
  struct level_trait<LOG_INFO>
  {
    static constexpr int enable = true;
    static constexpr const char* name = "INFO";
  };

  template<>
  struct level_trait<LOG_WARNING>
  {
    static constexpr int enable = true;
    static constexpr const char* name = "WARN";
  };

  template<>
  struct level_trait<LOG_ERR>
  {
    static constexpr int enable = true;
    static constexpr const char* name = "ERROR";
  };

  template<>
  struct level_trait<LOG_DEBUG>
  {
    static constexpr int enable = kLogDebug;
    static constexpr const char* name = "DEBUG";
  };

  template<>
  struct level_trait<LOG_NOTICE>
  {
    static constexpr int enable = kLogDebug;
    static constexpr const char* name = "TRACE";
  };

  template<int level, typename ...Args>
  inline void log_message(const char* format, Args&& ...args) {
    if constexpr (!level_trait<level>::enable)
      return;

    timespec ts;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);

    std::string msg;
    if constexpr (sizeof...(args) == 0) {
      msg = format;
    }
    else {
      int count = snprintf(msg.data(), 0, format, args...);
      msg.resize(count + 1);
      snprintf(msg.data(), msg.size(), format, args...);
    }

    std::stringstream ss;
    ss << ts.tv_sec << '.' << ts.tv_nsec << " [" << level_trait<level>::name << "] " << msg;

    if constexpr (level == LOG_ERR)
      fprintf(stderr, "%s\n", ss.str().data());
    else
      fprintf(stdout, "%s\n", ss.str().data());
  }
}

#define vDebug(fmt, ...) ::core::details::log_message<LOG_DEBUG>(fmt, ##__VA_ARGS__)
#define vInfo(fmt, ...) ::core::details::log_message<LOG_INFO>(fmt, ##__VA_ARGS__)
#define vWarning(fmt, ...) ::core::details::log_message<LOG_WARNING>(fmt, ##__VA_ARGS__)
#define vError(fmt, ...) ::core::details::log_message<LOG_ERR>(fmt, ##__VA_ARGS__)
#define vTrace(fmt, ...) ::core::details::log_message<LOG_NOTICE>(fmt, ##__VA_ARGS__)

#endif

#define __streamLog(level, msg)                         \
  do {                                                  \
    std::string s = (std::stringstream() << msg).str(); \
    v##level("%s", s.data());                           \
  } while (false)

#define mDebug(msg) __streamLog(Debug, msg)
#define mInfo(msg) __streamLog(Info, msg)
#define mWarning(msg) __streamLog(Warning, msg)
#define mError(msg) __streamLog(Error, msg)
#define mTrace(msg) __streamLog(Trace, msg)
