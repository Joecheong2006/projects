#ifndef _CORE_LOG_H_
#define _CORE_LOG_H_

typedef enum {
	LogLevelInfo = 0,
	LogLevelWarn,
	LogLevelDebug,
	LogLevelTrace,
	LogLevelError,
	LogLevelFatal,
} log_level;

void log_msg(log_level level, const char* msg, ...);
void log_level_msg(log_level level, const char* msg, ...);

#define LOG_INFO_MSG(...) log_msg(LogLevelInfo, __VA_ARGS__)
#define LOG_WARN_MSG(...) log_msg(LogLevelWarn, __VA_ARGS__)
#define LOG_TRACE_MSG(...) log_msg(LogLevelTrace, __VA_ARGS__)
#define LOG_ERROR_MSG(...) log_msg(LogLevelError, __VA_ARGS__)
#define LOG_FATAL_MSG(...) log_msg(LogLevelFatal, __VA_ARGS__)

#define LOG_INFO(...) log_level_msg(LogLevelInfo, __VA_ARGS__)
#define LOG_WARN(...) log_level_msg(LogLevelWarn, __VA_ARGS__)
#define LOG_TRACE(...) log_level_msg(LogLevelTrace, __VA_ARGS__)
#define LOG_ERROR(...) log_level_msg(LogLevelError, __VA_ARGS__)
#define LOG_FATAL(...) log_level_msg(LogLevelFatal, __VA_ARGS__)

#if defined(DEBUG)
#define LOG_DEBUG(...) log_level_msg(LogLevelDebug, __VA_ARGS__)
#define LOG_DEBUG_MSG(...) log_msg(LogLevelDebug, __VA_ARGS__)
#else
#define LOG_DEBUG(...)
#define LOG_DEBUG_MSG(...)
#endif

#endif
