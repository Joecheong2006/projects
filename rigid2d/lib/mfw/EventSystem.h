#include "mfwpch.h"

#pragma once
#include "util.h"
#include "mfwlog.h"

#define LOG_EVENT_INFO(event) LOG_INFO("{}\n", static_cast<const Event&>(event))

namespace mfw {
    enum struct EventType : u32 {
        UnDefine,
        WindowCreate, WindowClose, WindowDestroy,
        WindowMaximize, WindowMinimize, WindowMove, WindowResize,
        WindowFocus, WindowNotFocus, 
        Key, MouseButton, CursorMove, MouseScroll
    };

#define SET_EVENT_NORMAL_BEHAVIOUR(eventType) \
        virtual std::string getEventTypeAsString() const override { return #eventType; } \
        static EventType Type() { return eventType; } \

    class Event {
        friend Log::Pattern<mfw::Event>;
    public:
        Event(EventType eventType)
            : m_eventType(eventType)
        {}
        static EventType Type();
        EventType getEventType() const { return m_eventType; }
        virtual std::string getEventTypeAsString() const = 0;

    protected:
        virtual void log() const {
            LOG_INFO("[EVENT][{}]", getEventTypeAsString());
        }
        EventType m_eventType;

    };

    class EventListener {
        using EventFunc = std::function<bool(const Event&)>;
    public:
        template <class T>
        void listen(const Event& event) {
            if (event.getEventType() == T::Type()) {
                eventCallBackMap[T::Type()](event);
            }
        }

        template <class T>
        void addEventFunc(EventFunc eventFunc) {
            eventCallBackMap[T::Type()] = eventFunc;
        }

    private:
        std::unordered_map<EventType, EventFunc> eventCallBackMap;

    };
}

namespace Log {
    template <>
    struct Pattern<mfw::Event> {
        static void Log(const mfw::Event& value, const std::string& format) {
            (void)format;
            value.log();
        }
    };
}

