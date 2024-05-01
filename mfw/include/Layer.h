#include "mfwpch.h"
#pragma once

namespace mfw {
    class Event;
    class WindowResizeEvent;
    class WindowCloseEvent;
    class CursorMoveEvent;
    class KeyEvent;
    class MouseButtonEvent;
    class MouseScrollEvent;
    class WindowFocusEvent;
    class WindowNotFocusEvent;

    class Layer {
    public:
        Layer(const std::string& name): name(name) {}
        virtual ~Layer() = default;
        bool handleEvent(const Event& event);
        virtual void OnStart() {}
        virtual void OnUpdate() {}
        virtual bool OnInputKey(const KeyEvent& event) { (void)event; return false; }
        virtual bool OnMouseButton(const MouseButtonEvent& event) { (void)event; return false; }
        virtual bool OnMouseScroll(const MouseScrollEvent& event) { (void)event; return false; }
        virtual bool OnWindowResize(const WindowResizeEvent& event) { (void)event; return false; }
        virtual bool OnWindowClose(const WindowCloseEvent& event) { (void)event; return false; }
        virtual bool OnCursorMove(const CursorMoveEvent& event) { (void)event; return false; }
        virtual bool OnWindowFocus(const WindowFocusEvent& event) { (void)event; return false; }
        virtual bool OnWindowNotFocus(const WindowNotFocusEvent& event) { (void)event; return false; }

        const std::string name;

    };
}

