#pragma once

#include "EventSystem.h"
#include "WindowsWindow.h"
#include "OpenglContext.h"

namespace mfw {
    class Application {
    public:
        Application();
        virtual ~Application();
        void run();

    private:
        void handleEvent(const Event& event);
        void update();

        WindowsWindow m_window;
        OpenglContext openglContext;

    };

    Application* CreateApplication();
}

