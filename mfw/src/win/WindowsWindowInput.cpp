#include "win/WindowsWindowInput.h"
#include "Application.h"
#include "win/WindowsWindow.h"
#include "KeyCode.h"

namespace mfw {
    Input* Input::Instance = new WindowsWindowInput();

    WindowsWindowInput::WindowsWindowInput() {
        keymap[VK_SPACE] = MF_KEY_SPACE;
        keymap[VK_OEM_7] = MF_KEY_APOSTROPHE;
        keymap[VK_OEM_COMMA] = MF_KEY_COMMA;
        keymap[VK_OEM_MINUS] = MF_KEY_MINUS;
        keymap[VK_OEM_PERIOD] = MF_KEY_PERIOD;
        keymap[VK_OEM_2] = MF_KEY_SLASH;
        keymap[VK_OEM_1] = MF_KEY_SEMICOLON;
        keymap[VK_OEM_PLUS] = MF_KEY_EQUAL;
        keymap[VK_OEM_4] = MF_KEY_LEFT_BRACKET;
        keymap[VK_OEM_5] = MF_KEY_BACKSLASH;
        keymap[VK_OEM_6] = MF_KEY_RIGHT_BRACKET;
        keymap[VK_OEM_3] = MF_KEY_GRAVE_ACCENT;

        keymap['0'] = MF_KEY_0;
        keymap['1'] = MF_KEY_1;
        keymap['2'] = MF_KEY_2;
        keymap['3'] = MF_KEY_3;
        keymap['4'] = MF_KEY_4;
        keymap['5'] = MF_KEY_5;
        keymap['6'] = MF_KEY_6;
        keymap['7'] = MF_KEY_7;
        keymap['8'] = MF_KEY_8;
        keymap['9'] = MF_KEY_9;

        keymap['A'] = MF_KEY_A;
        keymap['B'] = MF_KEY_B;
        keymap['C'] = MF_KEY_C;
        keymap['D'] = MF_KEY_D;
        keymap['E'] = MF_KEY_E;
        keymap['F'] = MF_KEY_F;
        keymap['G'] = MF_KEY_G;
        keymap['H'] = MF_KEY_H;
        keymap['I'] = MF_KEY_I;
        keymap['J'] = MF_KEY_J;
        keymap['K'] = MF_KEY_K;
        keymap['L'] = MF_KEY_L;
        keymap['M'] = MF_KEY_M;
        keymap['N'] = MF_KEY_N;
        keymap['O'] = MF_KEY_O;
        keymap['P'] = MF_KEY_P;
        keymap['Q'] = MF_KEY_Q;
        keymap['R'] = MF_KEY_R;
        keymap['S'] = MF_KEY_S;
        keymap['T'] = MF_KEY_T;
        keymap['U'] = MF_KEY_U;
        keymap['V'] = MF_KEY_V;
        keymap['W'] = MF_KEY_W;
        keymap['X'] = MF_KEY_X;
        keymap['Y'] = MF_KEY_Y;
        keymap['Z'] = MF_KEY_Z;

        keymap[VK_ESCAPE] = MF_KEY_ESCAPE;
        keymap[VK_RETURN] = MF_KEY_ENTER;
        keymap[VK_TAB] = MF_KEY_TAB;
        keymap[VK_BACK] = MF_KEY_BACKSPACE;
        keymap[VK_INSERT] = MF_KEY_INSERT;
        keymap[VK_DELETE] = MF_KEY_DELETE;
        keymap[VK_RIGHT] = MF_KEY_RIGHT;
        keymap[VK_LEFT] = MF_KEY_LEFT;
        keymap[VK_DOWN] = MF_KEY_DOWN;
        keymap[VK_UP] = MF_KEY_UP;
        keymap[VK_PRIOR] = MF_KEY_PAGE_UP;
        keymap[VK_NEXT] = MF_KEY_PAGE_DOWN;
        keymap[VK_HOME] = MF_KEY_HOME;
        keymap[VK_END] = MF_KEY_END;
        keymap[VK_CAPITAL] = MF_KEY_CAPS_LOCK;
        keymap[VK_SCROLL] = MF_KEY_SCROLL_LOCK;
        keymap[VK_NUMLOCK] = MF_KEY_NUM_LOCK;
        keymap[VK_SNAPSHOT] = MF_KEY_PRINT_SCREEN;
        keymap[VK_PAUSE] = MF_KEY_PAUSE;

        keymap[VK_F1]  = MF_KEY_F1;
        keymap[VK_F2]  = MF_KEY_F2;
        keymap[VK_F3]  = MF_KEY_F3;
        keymap[VK_F4]  = MF_KEY_F4;
        keymap[VK_F5]  = MF_KEY_F5;
        keymap[VK_F6]  = MF_KEY_F6;
        keymap[VK_F7]  = MF_KEY_F7;
        keymap[VK_F8]  = MF_KEY_F8;
        keymap[VK_F9]  = MF_KEY_F9;
        keymap[VK_F10] = MF_KEY_F10;
        keymap[VK_F11] = MF_KEY_F11;
        keymap[VK_F12] = MF_KEY_F12;
        keymap[VK_F13] = MF_KEY_F13;
        keymap[VK_F14] = MF_KEY_F14;
        keymap[VK_F15] = MF_KEY_F15;
        keymap[VK_F16] = MF_KEY_F16;
        keymap[VK_F17] = MF_KEY_F17;
        keymap[VK_F18] = MF_KEY_F18;
        keymap[VK_F19] = MF_KEY_F19;
        keymap[VK_F20] = MF_KEY_F20;
        keymap[VK_F21] = MF_KEY_F21;
        keymap[VK_F22] = MF_KEY_F22;
        keymap[VK_F23] = MF_KEY_F23;
        keymap[VK_F24] = MF_KEY_F24;

        keymap[VK_LSHIFT] = MF_KEY_LEFT_SHIFT;
        keymap[VK_LCONTROL] = MF_KEY_LEFT_CONTROL;
        keymap[VK_LMENU] = MF_KEY_LEFT_ALT;
        keymap[VK_LWIN] = MF_KEY_LEFT_SUPER;

        keymap[VK_RSHIFT] = MF_KEY_RIGHT_SHIFT;
        keymap[VK_RCONTROL] = MF_KEY_RIGHT_CONTROL;
        keymap[VK_RMENU] = MF_KEY_RIGHT_ALT;
        keymap[VK_RWIN] = MF_KEY_RIGHT_SUPER;

        keymap[VK_SHIFT] = MF_KEY_LEFT_SHIFT;
        keymap[VK_CONTROL] = MF_KEY_LEFT_CONTROL;
        keymap[VK_MENU] = MF_KEY_LEFT_ALT;
    }              

    bool WindowsWindowInput::KeyPressImpl(const i32& key) {
        WindowsWindow* window = static_cast<WindowsWindow*>(Application::Get().GetWindow().getNativeWindow());
        //ASSERT(key > 0 && key < 256);
        return window->keys[key];
    }

    bool WindowsWindowInput::MouseButtonDownImpl(const i32& button) {
        WindowsWindow* window = static_cast<WindowsWindow*>(Application::Get().GetWindow().getNativeWindow());
        return window->mouse.buttons[button] && window->mouse.actions[button] == KeyMode::Down;
    }

    const std::pair<i32, i32> WindowsWindowInput::GetMouseImpl() {
        WindowsWindow* window = static_cast<WindowsWindow*>(Application::Get().GetWindow().getNativeWindow());
        return { window->mouse.x, window->mouse.y };
    }

    i32 WindowsWindowInput::GetKeyCodeImpl(i32 key) {
        if (keymap.find(key) != keymap.end()) {
            return keymap[key];
        }
        return -1;
    }

}

