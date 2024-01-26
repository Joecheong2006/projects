#pragma once
#include "Window.h"

namespace mfw {
    class Application {
    public:
        Application();
        virtual ~Application();
        void run();

        Window* GetWindow() const { return m_window; }

    public:
        static Application* Get() { return Instance; }

    protected:
        inline virtual void Update() {}
        inline virtual void OnInputKey(const KeyEvent& event) { (void)event; }
        inline virtual void OnMouseButton(const MouseButtonEvent& event) { (void)event; }
        inline virtual void OnMouseScroll(const MouseScrollEvent& event) { (void)event; }

        void Terminate() { m_window->close(); }

    private:
        void Eventhandle(const Event& event);
        inline void InputHandle(const KeyEvent& event) { OnInputKey(event); }
        inline void MouseButtonHandle(const MouseButtonEvent& event) { OnMouseButton(event); }
        inline void MouseScrollHandle(const MouseScrollEvent& event) { OnMouseScroll(event); }

        Window* m_window;
        EventListener eventListener;

        static Application* Instance;

    };

    Application* CreateApplication();

}

