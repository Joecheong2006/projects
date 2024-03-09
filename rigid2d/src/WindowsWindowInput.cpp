#include "WindowsWindowInput.h"
#include "Application.h"
#include "WindowsWindow.h"

namespace mfw {
    Input* Input::Instance = new WindowsWindowInput();

    bool WindowsWindowInput::KeyPressImpl(const i32& key) {
        WindowsWindow* window = static_cast<WindowsWindow*>(Application::Get()->GetWindow()->getNativeWindow());
        return window->keys[key];
    }

    bool WindowsWindowInput::MouseButtonDownImpl(const i32& button) {
        WindowsWindow* window = static_cast<WindowsWindow*>(Application::Get()->GetWindow()->getNativeWindow());
        return window->mouse.buttons[button] && window->mouse.actions[button] == KeyMode::Down;
    }

    const std::pair<i32, i32> WindowsWindowInput::GetMouseImpl() {
        WindowsWindow* window = static_cast<WindowsWindow*>(Application::Get()->GetWindow()->getNativeWindow());
        return { window->mouse.x, window->mouse.y };
    }

}

