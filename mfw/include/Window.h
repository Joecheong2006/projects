#include "mfwpch.h"
#pragma once

#include "util.h"

namespace mfw {
    class Event;
    struct WindowState {
        WindowState()
            : isRunning(false), isVSync(false)
        {}
        WindowState(const std::string& title, i32 x, i32 y, i32 width = 960, i32 height = 640)
            : title(std::wstring(title.begin(), title.end())), x(x), y(y), width(width), height(height), m_callBackFunc([](const Event&) {})
        {}
        WindowState(const std::wstring& title, i32 x, i32 y, i32 width = 960, i32 height = 640)
            : title(title), x(x), y(y), width(width), height(height), m_callBackFunc([](const Event&) {})
        {}

        std::wstring title;
        i32 x, y, width, height;
        bool isRunning, isVSync;
        std::function<void(const Event&)> m_callBackFunc;
    };

    enum class WindowMode : i32 {
        Show,
        Hide,
        Minimize,
        Maximize,
        NoActive,
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
        virtual const wchar_t* title() const = 0;
        virtual void setVSync(bool enable) = 0;
        virtual void setEventCallBack(const std::function<void(const Event&)>& callBackFunction) = 0;
        virtual void* getNativeWindow() = 0;
        virtual void close() = 0;
        virtual void showCursor(bool show = true) = 0;
        virtual void setCursorPos(const u32 x, const u32 y) = 0;
        virtual void swapBuffers() = 0;
        virtual void setFullScreen(bool enable) = 0;
        virtual void setPosition(i32 x, i32 y) = 0;
        virtual void setSize(i32 width, i32 height) = 0;
        virtual void setMode(WindowMode mode) = 0;
        virtual void setFocus() = 0;

        static Window* Create(const WindowState& state);

    };
}

