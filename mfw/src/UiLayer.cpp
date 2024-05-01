#include "UiLayer.h"
#include "InputEvent.h"
#include "WindowEvent.h"

#include "imgui/imgui.h"

namespace mfw {
    void UiLayer::OnUpdate() {
        ImGui::Begin(name.c_str());
        focus = OnUiRender();
        ImGui::End();
    }

    bool UiLayer::handleEvent(const Event& event) {
        switch (event.getEventType()) {
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
            default:
                LOG_WARN("[UILAYER][UNSUPPORT HANDLE EVENT TYPE][{}]\n", event.getEventTypeAsString());
                break;
        }
        return false;
    }
}

