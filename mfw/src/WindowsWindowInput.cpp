#include "WindowsWindowInput.h"
#include "Application.h"
#include "WindowsWindow.h"

namespace mfw {
    mfw::Input* mfw::Input::instance = new WindowsWindowInput;

    bool WindowsWindowInput::KeyPressImpl(const i32& key) {
        WindowsWindow* window = static_cast<WindowsWindow*>(Application::Get()->GetWindow()->GetNativeWindow());
        return window->keys[key];
    }

    bool WindowsWindowInput::MouseButtonDownImpl(const i32& button) {
        WindowsWindow* window = static_cast<WindowsWindow*>(Application::Get()->GetWindow()->GetNativeWindow());
        return window->mouse.buttons[button] && window->mouse.actions[button] == KeyMode::Down;
    }

}

