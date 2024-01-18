#include "WindowsWindow.h"

#include "mfwlog.h"
#include "WindowEventSystem.h"
#include <locale>

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

    void WindowsWindow::handleMessage(UINT message, WPARAM wparam, LPARAM lparam) {
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

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
                WORD key = LOWORD(wparam);
                WORD flags = HIWORD(lparam);
                WORD scandcode = LOBYTE(flags);
                if ((flags & KF_EXTENDED) == KF_EXTENDED) {
                    scandcode = MAKEWORD(scandcode, 0xe0);
                }
                BOOL keyRepeat = ((flags & KF_REPEAT) == KF_REPEAT) * 2;
                BOOL keyDown = !keyRepeat;
                BOOL keyRelease = ((flags & KF_UP) == KF_UP);
                KeyMode mode = static_cast<KeyMode>(keyRepeat + keyDown + keyRelease);
                const Event& e = WindowKeyEvent(key, scandcode, mode);
                m_state.m_callBackFunc(e);
            } break;

            return;
        }
    }

    WindowsWindow::WindowsWindow()
    {}

    void WindowsWindow::initialize(const WindowState& state) {
        m_state = state;
        registerWindowClass();
        createWindowsWindow();
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
                m_state.style,
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
