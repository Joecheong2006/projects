#pragma once

#include "util.h"
#include "log.h"

namespace mfw {
    enum struct EventType : u32 {
        UnDefine, WindowCreate, WindowClose, WindowDestroy,
        WindowMaximize, WindowMinimize, WindowMove, WindowResize,
        WindowFocus, WindowNotFocus,
    };

#define SET_EVENT_NORMAL_BEHAVIOUR(eventType) \
        virtual std::string getEventTypeAsString() const override { return #eventType; } \
        static EventType Type() { return eventType; } \

    class Event {
    public:
        Event(EventType eventType)
            : m_eventType(eventType)
        {}
        static EventType Type();
        EventType getEventType() const { return m_eventType; }
        virtual std::string getEventTypeAsString() const = 0;

        friend std::ostream& operator<<(std::ostream& os, const mfw::Event& event) {
            event.log();
            return os;
        }

    protected:
        virtual void log() const {
            LOG_INFO(getEventTypeAsString());
        }
        EventType m_eventType;

    };

    class EventDispatcher {
    public:
        template <class T = Event>
        static void Dispatch(const Event& event) {
            if (event.getEventType() == T::Type()) {
                LOG_INFOLN(event);
            }
        }
    };

}

