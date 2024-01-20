#pragma once

#include "WindowEvent.h"
#include "InputEvent.h"

namespace mfw {
    struct WindowState {
        WindowState()
            : isRunning(false), isVSync(false)
        {}
        WindowState(std::string title, i32 x, i32 y, i32 width = 960, i32 height = 640)
            : title(title), x(x), y(y), width(width), height(height), m_callBackFunc([](const Event&) {})
        {}

        std::string title;
        i32 x, y, width, height;
        bool isRunning, isVSync;
        std::function<void(const Event&)> m_callBackFunc;
    };

    struct Window {
    public:
        virtual ~Window() {}
        virtual void update() = 0;
        virtual bool isRunning() const = 0;
        virtual i32 width() const = 0;
        virtual i32 height() const = 0;
        virtual i32 x() const = 0;
        virtual i32 y() const = 0;
        virtual const char* title() const = 0;
        virtual void setVSync(bool enable) = 0;
        virtual void setEventCallBack(const std::function<void(const Event&)>& callBackFunction) = 0;
        virtual void* getNativeWindow() = 0;
        virtual void close() = 0;
        virtual void showCursor() = 0;
        virtual void hideCursor() = 0;
        virtual void setCursorPos(const u32 x, const u32 y) = 0;
        virtual void swapBuffers() = 0;

        static Window* Create(const WindowState& state);

    };
}

