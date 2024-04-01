#pragma once

#include "OpenglContext.h"
#include "win/WindowsWindow.h"

namespace mfw {
    void GLClearError();
    bool GLLogCall(const char* file, int line, const char* func);
    class WindowsOpenglContext : public OpenglContext {
    public:
        virtual void CreateMordenImpl(Window* window, i32 major, i32 minor) override;
        virtual void ReleaseImpl() override;

    private:
        HGLRC m_hglrc;

    };
}

