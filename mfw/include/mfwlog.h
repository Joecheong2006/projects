#pragma once

#include "map.h"
#define _VOID(x) (void)(x);
#define TOVOID(...) MAP(_VOID, __VA_ARGS__)

#include "logger.h"
#if defined(DEBUG) || defined(_DEBUG)
#define ASSERT(x) if(!(x)) __debugbreak();
#define LOG_TRACE(...) Log::Trace(__VA_ARGS__);
#define LOG_DEBUG(...) Log::Debug(__VA_ARGS__);
#define LOG_INFO(...) Log::Info(__VA_ARGS__);
#define LOG_WARN(...) Log::Warn(__VA_ARGS__);
#define LOG_ERROR(...) Log::Error(__VA_ARGS__);
#define LOG_FATAL(...) Log::Fatal(__VA_ARGS__);

#else
#define ASSERT(x)
#define LOG_TRACE(...) TOVOID(__VA_ARGS__)
#define LOG_DEBUG(...) TOVOID(__VA_ARGS__)
#define LOG_INFO(...)  TOVOID(__VA_ARGS__)
#define LOG_WARN(...)  TOVOID(__VA_ARGS__)
#define LOG_ERROR(...) TOVOID(__VA_ARGS__)
#define LOG_FATAL(...) TOVOID(__VA_ARGS__)
#endif
