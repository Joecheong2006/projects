#pragma once
#include "util.h"
#include "mfwlog.h"

#define COLOR(val) ((u32)val >> 16) / 255.0, (((u32)val << 16) >> 24) / 255.0, (((u32)val << 24) >> 24) / 255.0

namespace mfw {
    void GLClearError();
    bool GLLogCall(const char* file, i32 line, const char* func);
}

#if defined(DEBUG)
#define GLCALL(x) mfw::GLClearError(); \
    x;\
    ASSERT(mfw::GLLogCall(__FILE__, __LINE__, #x))
#else
#define GLCALL(x) x
#endif

