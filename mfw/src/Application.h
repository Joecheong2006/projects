#pragma once
#include "Window.h"
#include <mfwpch.h>

namespace mfw {
    class Application {
    public:
        Application();
        virtual ~Application();
        void run();

        Window* GetWindow() { return m_window; }

    public:
        static Application* Get() { return Instance; }

    private:
        void Eventhandle(const Event& event);
        void InputHandle(const KeyEvent& event);
        void MouseButtonHandle(const MouseButtonEvent& event);
        void MouseScrollHandle(const MouseScrollEvent& event);
        virtual void Update();

        Window* m_window;
        EventListener eventListener;

        static Application* Instance;

    };

    Application* CreateApplication();

}

