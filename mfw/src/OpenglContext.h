#pragma once

#include "glad/gl.h"
#include "Window.h"

namespace mfw {
    class OpenglContext {
        friend class Application;
    public:
        virtual ~OpenglContext() {}
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

