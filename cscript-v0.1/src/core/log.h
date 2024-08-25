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

#define LOG_INFO(msg, ...) log_level_msg(LogLevelInfo, msg, __VA_ARGS__)
#define LOG_WARN(msg, ...) log_level_msg(LogLevelWarn, msg, __VA_ARGS__)
#define LOG_TRACE(msg, ...) log_level_msg(LogLevelTrace, msg, __VA_ARGS__)
#define LOG_ERROR(msg, ...) log_level_msg(LogLevelError, msg, __VA_ARGS__)
#define LOG_FATAL(msg, ...) log_level_msg(LogLevelFatal, msg, __VA_ARGS__)

#if defined(DEBUG)
#define LOG_DEBUG(msg, ...) log_level_msg(LogLevelDebug, msg, __VA_ARGS__)
#else
#define LOG_DEBUG(msg, ...)
#endif

#endif
