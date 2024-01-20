#pragma once

#define GLEW_STATIC 1
#include <GL/glew.h>
#include <GL/wglew.h>

#include "Window.h"

namespace mfw {
    class OpenglContext {
    public:
        static void CreateMorden(Window* window) { Instance->CreateMordenImpl(window); }
        static void CreateOld() { Instance->CreateOldImpl(); }
        static void Release() { Instance->ReleaseImpl(); }

    private:
        virtual void CreateMordenImpl(Window* window) = 0;
        virtual void CreateOldImpl() = 0;
        virtual void ReleaseImpl() = 0;

        static OpenglContext* Instance;

    };
}

