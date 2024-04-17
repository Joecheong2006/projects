#include "Layer.h"

#include "InputEvent.h"
#include "WindowEvent.h"

namespace mfw {
    bool Layer::handleEvent(const Event& event) {
        switch (event.getEventType()) {
            case EventType::WindowResize:
                return OnWindowResize(static_cast<const WindowResizeEvent&>(event));
                break;
            case EventType::WindowClose:
                return OnWindowClose(static_cast<const WindowCloseEvent&>(event));
                break;
            case EventType::CursorMove:
                return OnCursorMove(static_cast<const CursorMoveEvent&>(event));
                break;
            case EventType::Key:
                return OnInputKey(static_cast<const KeyEvent&>(event));
                break;
            case EventType::MouseButton:
                return OnMouseButton(static_cast<const MouseButtonEvent&>(event));
                break;
            case EventType::MouseScroll:
                return OnMouseScroll(static_cast<const MouseScrollEvent&>(event));
                break;
            case EventType::WindowFocus:
                return OnWindowFocus(static_cast<const WindowFocusEvent&>(event));
                break;
            case EventType::WindowNotFocus:
                return OnWindowNotFocus(static_cast<const WindowNotFocusEvent&>(event));
                break;
            default:
                LOG_WARN("[LAYER][UNSUPPORT HANDLE EVENT TYPE][{}]\n", event.getEventTypeAsString());
                break;
        }
        return false;
    }
}

