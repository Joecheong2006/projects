#include "WindowsWindow.h"

#include "mfwlog.h"
#include "WindowEvent.h"
#include "InputEvent.h"

#include "Clock.h"

#include <windowsx.h>

#define GETLOWORD(l) ((i16) (((i64) (l)) & 0xffff))
#define GETHIWORD(l) ((i16) ((((i64) (l)) >> 16) & 0xffff))

namespace mfw {
    const char* WindowsWindow::windowClassName = "__@@WindowClassName";

    LRESULT CALLBACK WindowsWindow::WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
        if (message == WM_CREATE) {
            CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lparam);
            WindowsWindow* window = reinterpret_cast<WindowsWindow*>(create->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
        }

        WindowsWindow* window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));;
        if (window) {
            window->handleMessage(message, wparam, lparam);
        }
        return DefWindowProc(hwnd, message, wparam, lparam);
    }

    void WindowsWindow::handleMessage(const UINT& message, const WPARAM& wparam, const LPARAM& lparam) {
        switch (message) {
        case WM_CREATE: {
                m_state.isRunning = true;
                m_state.m_callBackFunc(WindowCreateEvent());
            } break;

        case WM_CLOSE: {
                m_state.m_callBackFunc(WindowCloseEvent());
            } break;

        case WM_DESTROY: {
                m_state.m_callBackFunc(WindowDestroyEvent());
                m_state.isRunning = false;
            } return;

        case WM_MOVE: {
                m_state.x = GETLOWORD(lparam);
                m_state.y = GETHIWORD(lparam);
                m_state.m_callBackFunc(WindowMoveEvent(m_state.x, m_state.y));
            } break;

        case WM_SIZE: {
                m_state.width = GETLOWORD(lparam);
                m_state.height = GETHIWORD(lparam);
                m_state.m_callBackFunc(WindowResizeEvent(m_state.width, m_state.height));
                if (wparam == SIZE_MAXIMIZED)
                    m_state.m_callBackFunc(WindowMaximizeEvent());
                else if (wparam == SIZE_MINIMIZED)
                    m_state.m_callBackFunc(WindowMinimizeEvent());
            } break;

        case WM_SETFOCUS: {
                m_state.m_callBackFunc(WindowFocusEvent());
            } break;

        case WM_KILLFOCUS: {
                m_state.m_callBackFunc(WindowNotFocusEvent());
            } break;

        case WM_CHAR: {

            } break;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
                WORD key = LOWORD(wparam);
                WORD flags = HIWORD(lparam);
                WORD scancode = LOBYTE(flags);
                if ((flags & KF_EXTENDED) == KF_EXTENDED) {
                    scancode = MAKEWORD(scancode, 0xe0);
                }
                BOOL keyRepeat = ((flags & KF_REPEAT) == KF_REPEAT) * 2;
                BOOL keyDown = !keyRepeat;
                BOOL keyRelease = ((flags & KF_UP) == KF_UP);
                KeyMode mode = static_cast<KeyMode>(keyRepeat + keyDown + keyRelease);
                if (mode == KeyMode::Down || mode == KeyMode::Press)
                    keys[key] = true;
                else
                    keys[key] = false;
                m_state.m_callBackFunc(KeyEvent(key, scancode, mode));
            } break;

        case WM_XBUTTONUP: {
                mouse.buttons[MouseButton::X1 + GET_XBUTTON_WPARAM(wparam) - 1] = false;
                mouse.actions[MouseButton::X1 + GET_XBUTTON_WPARAM(wparam) - 1] = KeyMode::Release;
                m_state.m_callBackFunc(MouseButtonEvent(MouseButton::X1 + GET_XBUTTON_WPARAM(wparam) - 1, KeyMode::Release));
            } break;
        case WM_LBUTTONUP: {
                mouse.buttons[MouseButton::Left] = false;
                mouse.actions[MouseButton::Left] = KeyMode::Release;
                m_state.m_callBackFunc(MouseButtonEvent(MouseButton::Left, KeyMode::Release));
            } break;
        case WM_RBUTTONUP: {
                mouse.buttons[MouseButton::Right] = false;
                mouse.actions[MouseButton::Right] = KeyMode::Release;
                m_state.m_callBackFunc(MouseButtonEvent(MouseButton::Right, KeyMode::Release));
            } break;
        case WM_MBUTTONUP: {
                mouse.buttons[MouseButton::Middle] = false;
                mouse.actions[MouseButton::Middle] = KeyMode::Release;
                m_state.m_callBackFunc(MouseButtonEvent(MouseButton::Middle, KeyMode::Release));
            } break;

        case WM_XBUTTONDOWN: {
                mouse.buttons[MouseButton::X1 + GET_XBUTTON_WPARAM(wparam)] = true;
                mouse.actions[MouseButton::X1 + GET_XBUTTON_WPARAM(wparam)] = KeyMode::Down;
                m_state.m_callBackFunc(MouseButtonEvent(MouseButton::X1 + GET_XBUTTON_WPARAM(wparam) - 1, KeyMode::Down));
            } break;
        case WM_LBUTTONDOWN: {
                mouse.buttons[MouseButton::Left] = true;
                mouse.actions[MouseButton::Left] = KeyMode::Down;
                m_state.m_callBackFunc(MouseButtonEvent(MouseButton::Left, KeyMode::Down));
            } break;
        case WM_RBUTTONDOWN: {
                mouse.buttons[MouseButton::Right] = true;
                mouse.actions[MouseButton::Right] = KeyMode::Down;
                m_state.m_callBackFunc(MouseButtonEvent(MouseButton::Right, KeyMode::Down));
            } break;
        case WM_MBUTTONDOWN: {
                mouse.buttons[MouseButton::Middle] = true;
                mouse.actions[MouseButton::Middle] = KeyMode::Down;
                m_state.m_callBackFunc(MouseButtonEvent(MouseButton::Middle, KeyMode::Down));
            } break;

        case WM_MOUSEMOVE: {
                GetCursorPos(reinterpret_cast<PPOINT>(&mouse.ax));
                mouse.rx = mouse.ax - m_state.x;
                mouse.ry = mouse.ay - m_state.y;
                m_state.m_callBackFunc(CursorMoveEvent(mouse.rx, mouse.ry));
            } break;

        case WM_MOUSEHWHEEL: {
                auto delta = GET_WHEEL_DELTA_WPARAM(wparam) > 0 ? 1 : -1;
                m_state.m_callBackFunc(MouseScrollEvent(delta, 0));
            } break;

        case WM_MOUSEWHEEL: {
                auto delta = GET_WHEEL_DELTA_WPARAM(wparam) > 0 ? 1 : -1;
                m_state.m_callBackFunc(MouseScrollEvent(0, delta));
            } break;
            return;
        }
    }

    Window* Window::Create(const WindowState& state) {
        return new WindowsWindow(state);
    }

    WindowsWindow::WindowsWindow(const WindowState& state)
        : keys{}, mouse{}
    {
        START_CLOCK_DURATION("INIT WINDOW");
        m_state = state;
        registerWindowClass();
        createWindowsWindow();
        ShowWindow(m_hwnd, SW_NORMAL);
    }

    void WindowsWindow::registerWindowClass() {
        HINSTANCE instance = GetModuleHandle(NULL);
        WNDCLASSEX wc{};
        if (GetClassInfoEx(instance, windowClassName, &wc))
            return;
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.lpfnWndProc = &WindowsWindow::WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = windowClassName;
        wc.style = CS_OWNDC;

        RegisterClassEx(&wc);
    }

    void WindowsWindow::createWindowsWindow() {
        m_hwnd = CreateWindow(
                windowClassName,
                m_state.title.c_str(),
                WS_OVERLAPPEDWINDOW,
                0, 0, m_state.width, m_state.height,
                NULL,
                NULL,
                GetModuleHandle(NULL),
                this
                );

        m_hdc = GetDC(m_hwnd);

        LOG_INFO("WINDOW CREATE SUCCESS: {}\n", m_state.title);
    }

    void WindowsWindow::update() {
        processMessage();
    }

    void* WindowsWindow::GetNativeWindow() {
        return this;
    }

    void WindowsWindow::showCursor() {
        ShowCursor(true);
    }

    void WindowsWindow::hideCursor() {
        ShowCursor(false);
    }

    void WindowsWindow::setCursorPos(const u32 x, const u32 y) {
        SetCursorPos(m_state.x + x, m_state.y + y);
    }

    void WindowsWindow::processMessage() {
        MSG msg{};
        while(PeekMessage(&msg, m_hwnd, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    WindowsWindow::~WindowsWindow() {
        if (!m_hdc) {
            ReleaseDC(m_hwnd, m_hdc);
            m_hdc = nullptr;
        }
        if (!m_hwnd) {
            DestroyWindow(m_hwnd);
            m_hwnd = nullptr;
        }
        LOG_INFO("WINDOW CLOSE SUCCESS: {}\n", m_state.title);
    }

}
