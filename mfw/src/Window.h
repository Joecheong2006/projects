#pragma once

#include "util.h"
#include "EventSystem.h"
#include "mfwpch.h"

namespace mfw {
    struct WindowState {
        WindowState()
            : isRunning(false), isVSync(false)
        {}
        WindowState(char* title, i32 style, i32 x, i32 y, i32 width = 960, i32 height = 640)
            : title(title), style(style), x(x), y(y), width(width), height(height)
        {}

        std::string title;
        i32 style;
        i32 x, y, width, height;
        bool isRunning, isVSync;

        std::function<void(const Event&)> m_callBackFunction = [](const Event&) {};
    };

    struct Window {
    public:
        ~Window() {}
        virtual void initialize() = 0;
        virtual void update() = 0;
        virtual bool isRunning() const = 0;
        virtual i32 width() const = 0;
        virtual i32 height() const = 0;
        virtual i32 x() const = 0;
        virtual i32 y() const = 0;
        virtual i32 style() const = 0;
        virtual const char* title() const = 0;
        virtual void setVSync(bool enable) = 0;
        virtual void setEventCallBack(const std::function<void(const Event&)>& callBackFunction) = 0;
    };
}

