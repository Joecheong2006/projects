#include "WindowsWindow.h"

#include "mfwlog.h"
#include "WindowEventSystem.h"

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
                WORD code = LOWORD(wparam);
                WORD flags = HIWORD(lparam);
                WORD scancode = LOBYTE(flags);
                BOOL isExtendedKey = (flags & KF_EXTENDED) == KF_EXTENDED;
                if (isExtendedKey) {
                    scancode = MAKEWORD(scancode, 0xe0);
                }
                BOOL keyDown = (flags & KF_REPEAT) == KF_REPEAT;
                BOOL keyFirst = !keyDown;
                WORD repeatCount = LOWORD(lparam);
                BOOL keyRelease = (flags & KF_UP) == KF_UP;

                LOG_TRACE("{:03}\n", code);
                //LOG_TRACE("code[{d:03}] flags[{d:06}] scancode[{d:03}] EK[{d:1}] keyFirst[{d:1}] keyDown[{d:1}] repeatCount[{d:02}] keyRelease[{d:1}]\n", code, flags, scancode, isExtendedKey, keyFirst, keyDown, repeatCount, keyRelease);
            } break;

        //case WM_KEYDOWN:
        //case WM_SYSKEYDOWN: {
        //        LOG_TRACE("KEYDOWN: [{d}]\n", wparam);
        //    } break;

        //case WM_KEYUP:
        //case WM_SYSKEYUP: {
        //        LOG_TRACE("KEYUP:   [{d}]\n", wparam);
        //    } break;

        //case WM_CHAR:
        //case WM_SYSCHAR: {
        //        LOG_TRACE("KEYCHAR: '{c}'\n", wparam);
        //    } break;

        default:
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
        ReleaseDC(m_hwnd, m_hdc);
        DestroyWindow(m_hwnd);
        LOG_INFO("WINDOW CREATE SUCCESS: {}\n", m_state.title);
    }

}
