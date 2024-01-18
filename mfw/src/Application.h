#pragma once

#include "WindowsWindow.h"
#include "OpenglContext.h"
#include "WindowEventSystem.h"

namespace mfw {
    class Application {
    public:
        Application();
        virtual ~Application();
        void run();

    private:
        void handleEvent(const Event& event);
        void input(const WindowKeyEvent& event);
        void update();

        WindowsWindow m_window;
        OpenglContext openglContext;
        EventListener eventListener;

    };

    Application* CreateApplication();
}

