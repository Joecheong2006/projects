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
        f32 dt;

    protected:
        inline virtual void Init() {}
        inline virtual void Update() {}
        inline virtual void OnInputKey(const KeyEvent& event) { (void)event; }
        inline virtual void OnMouseButton(const MouseButtonEvent& event) { (void)event; }
        inline virtual void OnMouseScroll(const MouseScrollEvent& event) { (void)event; }
        inline virtual void OnWindowResize(const WindowResizeEvent& event) { (void)event; }
        inline virtual void OnWindowClose(const WindowCloseEvent& event) { (void)event; }
        inline virtual void OnCursorMove(const CursorMoveEvent& event) { (void)event; }

        void Terminate() { m_window->close(); }

    private:
        void Eventhandle(const Event& event);
        inline void InputHandle(const KeyEvent& event) { OnInputKey(event); }
        inline void MouseButtonHandle(const MouseButtonEvent& event) { OnMouseButton(event); }
        inline void MouseScrollHandle(const MouseScrollEvent& event) { OnMouseScroll(event); }
        inline void WindowResize(const WindowResizeEvent& event) { OnWindowResize(event); }
        inline void WindowClose(const WindowCloseEvent& event) { OnWindowClose(event); }
        inline void CursorMove(const CursorMoveEvent& event) { OnCursorMove(event); }

        Window* m_window;
        EventListener eventListener;

        static Application* Instance;

    };

    Application* CreateApplication();

}

