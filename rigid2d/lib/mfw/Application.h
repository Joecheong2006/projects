#pragma once
#include "Window.h"
#include "EventSystem.h"
#include "LayerSystem.h"
#include "UiLayerSystem.h"

namespace mfw {
    class WindowResizeEvent;
    class WindowCloseEvent;
    class CursorMoveEvent;
    class KeyEvent;
    class MouseButtonEvent;
    class MouseScrollEvent;
    class WindowFocusEvent;
    class WindowNotFocusEvent;

    class Application {
    public:
        Application();
        Application(const std::string& title, i32 width, i32 height);
        virtual ~Application();
        void run();

        Window& GetWindow() const { return *m_window; }
        void addLayer(Layer* layer);
        void addUiLayer(UiLayer* uiLayer);

    public:
        static Application& Get() { return *Instance; }

    protected:
        inline virtual void Start() {}
        inline virtual void Update() {}
        inline virtual bool OnInputKey(const KeyEvent& event) { (void)event; return false; }
        inline virtual bool OnMouseButton(const MouseButtonEvent& event) { (void)event; return false; }
        inline virtual bool OnMouseScroll(const MouseScrollEvent& event) { (void)event; return false; }
        inline virtual bool OnWindowResize(const WindowResizeEvent& event) { (void)event; return false; }
        inline virtual bool OnWindowClose(const WindowCloseEvent& event) { (void)event; return false; }
        inline virtual bool OnCursorMove(const CursorMoveEvent& event) { (void)event; return false; }
        inline virtual bool OnWindowFocus(const WindowFocusEvent& event) { (void)event; return false; }
        inline virtual bool OnWindowNotFocus(const WindowNotFocusEvent& event) { (void)event; return false; }

        void Terminate();

    private:
        void Eventhandle(const Event& event);
        inline bool InputHandle(const KeyEvent& event) { return OnInputKey(event); }
        inline bool MouseButtonHandle(const MouseButtonEvent& event) { return OnMouseButton(event); }
        inline bool MouseScrollHandle(const MouseScrollEvent& event) { return OnMouseScroll(event); }
        inline bool WindowResize(const WindowResizeEvent& event) { return OnWindowResize(event); }
        inline bool WindowClose(const WindowCloseEvent& event) { return OnWindowClose(event); }
        inline bool CursorMove(const CursorMoveEvent& event) { return OnCursorMove(event); }
        inline bool WindowFocus(const WindowFocusEvent& event) { return OnWindowFocus(event); }
        inline bool WindowNotFocus(const WindowNotFocusEvent& event) { return OnWindowNotFocus(event); }

        std::unique_ptr<Window> m_window;
        EventListener eventListener;
        LayerSystem layerSystem;
        UiLayerSystem uiLayerSystem;

        static Application* Instance;

    };

    Application* CreateApplication();

}

