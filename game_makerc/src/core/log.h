#ifndef _LOG_
#define _LOG_

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
