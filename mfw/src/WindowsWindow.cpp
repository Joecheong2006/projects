#include "WindowsWindow.h"

#include "log.h"
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
        if (window)
        {
            window->handleMessage(message, wparam, lparam);
        }
        return DefWindowProc(hwnd, message, wparam, lparam);
    }

    void WindowsWindow::handleMessage(UINT message, WPARAM wparam, LPARAM lparam) {
        switch (message) {
        case WM_CREATE: {
                m_state.isRunning = true;
                m_state.m_callBackFunction(WindowCreateEvent());
            } break;
        case WM_CLOSE: {
                m_state.isRunning = false;
                m_state.m_callBackFunction(WindowCloseEvent());
            } break;

        case WM_DESTROY: {
                m_state.m_callBackFunction(WindowDestroyEvent());
            } break;

        case WM_MOVE: {
                m_state.x = GETLOWORD(lparam);
                m_state.y = GETHIWORD(lparam);
                m_state.m_callBackFunction(WindowMoveEvent(m_state.x, m_state.y));
            } break;

        case WM_SIZE: {
                m_state.width = GETLOWORD(lparam);
                m_state.height = GETHIWORD(lparam);
                m_state.m_callBackFunction(WindowResizeEvent(m_state.width, m_state.height));
                if (wparam == SIZE_MAXIMIZED)
                    m_state.m_callBackFunction(WindowMaximizeEvent());
                else if (wparam == SIZE_MINIMIZED)
                    m_state.m_callBackFunction(WindowMinimizeEvent());
            } break;

        case WM_SETFOCUS: {
                m_state.m_callBackFunction(WindowFocusEvent());
            } break;

        case WM_KILLFOCUS: {
                m_state.m_callBackFunction(WindowNotFocusEvent());
            } break;

        default:
            return;
        }
    }

    WindowsWindow::WindowsWindow(const char* title, int width, int height, int style)
        : m_state((char*)title, style, 0, 0, width, height)
    {}
   
    void WindowsWindow::initialize() {
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
                m_state.style,
                0, 0, m_state.width, m_state.height,
                NULL,
                NULL,
                GetModuleHandle(NULL),
                this
                );

        m_hdc = GetDC(m_hwnd);

        LOG_INFOLN("WINDOW CREATE SUCCESS: ", m_state.title);
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
        LOG_INFOLN("WINDOW CREATE SUCCESS: ", m_state.title);
    }

}
