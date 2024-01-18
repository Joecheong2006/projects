#pragma once

#include "EventSystem.h"

namespace mfw {
    struct WindowCreateEvent : public Event {
        WindowCreateEvent()
            : Event(EventType::WindowCreate)
        {}
        SET_EVENT_NORMAL_BEHAVIOUR(EventType::WindowCreate);
    };

    struct WindowCloseEvent : public Event {
        WindowCloseEvent()
            : Event(EventType::WindowClose)
        {}
        SET_EVENT_NORMAL_BEHAVIOUR(EventType::WindowClose);
    };

    struct WindowDestroyEvent : public Event {
        WindowDestroyEvent()
            : Event(EventType::WindowDestroy)
        {}
        SET_EVENT_NORMAL_BEHAVIOUR(EventType::WindowDestroy);
    };

    struct WindowMaximizeEvent : public Event {
        WindowMaximizeEvent()
            : Event(EventType::WindowMaximize)
        {}
        SET_EVENT_NORMAL_BEHAVIOUR(EventType::WindowMaximize);
    };

    struct WindowMinimizeEvent : public Event {
        WindowMinimizeEvent()
            : Event(EventType::WindowMinimize)
        {}
        SET_EVENT_NORMAL_BEHAVIOUR(EventType::WindowMinimize);
    };

    struct WindowMoveEvent : public Event {
        WindowMoveEvent(i32 x, i32 y)
            : Event(EventType::WindowMove), x(x), y(y)
        {}
        SET_EVENT_NORMAL_BEHAVIOUR(EventType::WindowMove);
        const i32 x, y;
    private:
        virtual void log() const override {
            Event::log();
            LOG_INFO("[{}, {}]", x, y);
        }

    };

    struct WindowResizeEvent : public Event {
        WindowResizeEvent(i32 width, i32 height)
            : Event(EventType::WindowResize), width(width), height(height)
        {}
        SET_EVENT_NORMAL_BEHAVIOUR(EventType::WindowResize);
        const i32 width, height;
    private:
        virtual void log() const override {
            Event::log();
            LOG_INFO("[{}, {}]", width, height);
        }

    };

    struct WindowFocusEvent : public Event {
        WindowFocusEvent()
            : Event(EventType::WindowFocus)
        {}
        SET_EVENT_NORMAL_BEHAVIOUR(EventType::WindowFocus);
    };

    struct WindowNotFocusEvent : public Event {
        WindowNotFocusEvent()
            : Event(EventType::WindowNotFocus)
        {}
        SET_EVENT_NORMAL_BEHAVIOUR(EventType::WindowNotFocus);
    };

    enum KeyMode : u8 {
        Down = 1,
        Repeat,
        Release,
    };

    struct WindowKeyEvent : public Event {
        WindowKeyEvent(i32 key, i32 scandcode, KeyMode mode)
            : Event(EventType::WindowKey), key(key), scandcode(scandcode), mode(mode)
        {}
        SET_EVENT_NORMAL_BEHAVIOUR(EventType::WindowKey);

        const i32 key, scandcode;
        const KeyMode mode;
    private:
        virtual void log() const override {
            Event::log();
            LOG_INFO("key[{d:03}] scancode[{d:03}] mode[{}]", key, scandcode, static_cast<u8>(mode));
        }

    };

}

