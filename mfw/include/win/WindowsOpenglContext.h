#pragma once

#include "OpenglContext.h"
#include "win/WindowsWindow.h"

namespace mfw {
    void GLClearError();
    bool GLLogCall(const char* file, int line, const char* func);
    class WindowsOpenglContext : public OpenglContext {
    public:
        virtual void CreateMordenImpl(Window* window) override;
        virtual void CreateOldImpl() override;
        virtual void ReleaseImpl() override;

    private:
        HGLRC m_hglrc;

    };
}

