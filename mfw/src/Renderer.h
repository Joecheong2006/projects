#pragma once

#include "util.h"
#include "mfwlog.h"

namespace mfw {
    void GLClearError();
    bool GLLogCall(const char* file, i32 line, const char* func);
}

#if defined(DEBUG) || defined(_DEBUG)
#define GLCALL(x) mfw::GLClearError(); \
    x;\
    ASSERT(mfw::GLLogCall(__FILE__, __LINE__, #x))
#else
#define GLCALL(x) x
#endif

