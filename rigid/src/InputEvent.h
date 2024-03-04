#pragma once

#include "EventSystem.h"

namespace mfw {
    enum KeyMode : u8 {
        Down = 1,
        Press,
        Release,
    };

    struct KeyEvent : public Event {
        KeyEvent(i32 key, i32 scancode, KeyMode mode)
            : Event(EventType::Key), key(key), scancode(scancode), mode(mode)
        {}
        SET_EVENT_NORMAL_BEHAVIOUR(EventType::Key);

        const i32 key, scancode;
        const KeyMode mode;
    private:
        virtual void log() const override {
            Event::log();
            LOG_INFO("[key: {d:03} scancode: {d:03} mode: {}]", key, scancode, static_cast<u8>(mode));
        }

    };

    struct MouseButtonEvent : public Event {
        MouseButtonEvent(i32 button, KeyMode mode)
            : Event(EventType::MouseButton), button(button), mode(mode)
        {}
        SET_EVENT_NORMAL_BEHAVIOUR(EventType::MouseButton);
        
        const i32 button;
        const KeyMode mode;
    private:
        virtual void log() const override {
            Event::log();
            LOG_INFO("[button: {} mode: {}]", button, static_cast<u8>(mode));
        }
        
    };

    struct CursorMoveEvent : public Event {
        CursorMoveEvent(i32 x, i32 y)
            : Event(EventType::CursorMove), x(x), y(y)
        {}
        SET_EVENT_NORMAL_BEHAVIOUR(EventType::CursorMove);

        const i32 x, y;
    private:
        virtual void log() const override {
            Event::log();
            LOG_INFO("[x: {} y: {}]", x, y);
        }

    };

    struct MouseScrollEvent : public Event {
        MouseScrollEvent(i32 xdelta, i32 ydelta)
            : Event(EventType::MouseScroll), xdelta(xdelta), ydelta(ydelta)
        {}
        SET_EVENT_NORMAL_BEHAVIOUR(EventType::MouseScroll);
        
        const i32 xdelta, ydelta;
    private:
        virtual void log() const override {
            Event::log();
            LOG_INFO("[xdelta: {} ydelta: {}]", xdelta, ydelta);
        }
        
    };
}

