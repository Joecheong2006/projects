#pragma once

#include "WindowsWindow.h"
#define GLEW_STATIC 1
#include "glew/GL/glew.h"

namespace mfw {
    void GLClearError();
    bool GLLogCall(const char* file, int line, const char* func);
    class OpenglContext {
    public:
        void createOld();
        void createMorden(WindowsWindow* window);
        void release();

    private:
        HGLRC m_hglrc;

    };
}

