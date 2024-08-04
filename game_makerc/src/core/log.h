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

#endif
