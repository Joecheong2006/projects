#pragma once

#include "WindowsWindow.h"
#include <mfwpch.h>

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

