#include "mfwpch.h"
#pragma once
#include "Layer.h"

namespace mfw {
    class Event;
    class CursorMoveEvent;
    class KeyEvent;
    class MouseButtonEvent;
    class MouseScrollEvent;

    class UiLayer {
    public:
        UiLayer(const std::string& name = " "): name(name) {}
        virtual ~UiLayer() = default;
        void OnUpdate();
        bool handleEvent(const Event& event);
        bool OnInputKey(const KeyEvent& event) { (void)event; return focus; }
        bool OnMouseButton(const MouseButtonEvent& event) { (void)event; return focus; }
        bool OnMouseScroll(const MouseScrollEvent& event) { (void)event; return focus; }
        bool OnCursorMove(const CursorMoveEvent& event) { (void)event; return focus; }
        virtual bool OnUiRender() = 0;
        virtual void OnStart() {}

        std::string name;
        bool focus = false;

    };
}

