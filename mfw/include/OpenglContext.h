#pragma once

#include "glad/gl.h"
#include "Window.h"

namespace mfw {
    class OpenglContext {
        friend class Application;
    public:
        virtual ~OpenglContext() = default;
        static void CreateMorden(Window* window, i32 major, i32 minor) {
            Instance->CreateMordenImpl(window, major, minor);
        }
        static void Release() { Instance->ReleaseImpl(); }

    private:
        virtual void CreateMordenImpl(Window* window, i32 major, i32 minor) = 0;
        virtual void ReleaseImpl() = 0;

        static OpenglContext* Instance;

    };
}

