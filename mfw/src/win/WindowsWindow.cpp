#include "win/WindowsWindow.h"

#include "WindowEvent.h"
#include "Input.h"
#include "OpenglContext.h"
#include "glad/wgl.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"

#define GETLOWORD(l) ((i16) (((i64) (l)) & 0xffff))
#define GETHIWORD(l) ((i16) ((((i64) (l)) >> 16) & 0xffff))

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace mfw {
    const wchar_t* windowClassName = L"__@@WindowClassName";

    LRESULT CALLBACK WindowsWindow::WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
        if (message == WM_CREATE) {
            CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lparam);
            WindowsWindow* window = reinterpret_cast<WindowsWindow*>(create->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
        }

        if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wparam, lparam)) {
            return true;
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

        case WM_UNICHAR: {
                 return;
            }

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
                keys[key] = mode == KeyMode::Down || mode == KeyMode::Press;
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
                mouse.buttons[MouseButton::X1 + GET_XBUTTON_WPARAM(wparam) - 1] = true;
                mouse.actions[MouseButton::X1 + GET_XBUTTON_WPARAM(wparam) - 1] = KeyMode::Down;
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
                GetCursorPos(reinterpret_cast<PPOINT>(&mouse.x));
                mouse.x -= m_state.x;
                mouse.y -= m_state.y;
                m_state.m_callBackFunc(CursorMoveEvent(mouse.x, mouse.y));
            } break;

        case WM_MOUSEHWHEEL: {
                auto delta = GET_WHEEL_DELTA_WPARAM(wparam) > 0 ? 1 : -1;
                m_state.m_callBackFunc(MouseScrollEvent(delta, 0));
            } break;

        case WM_MOUSEWHEEL: {
                auto delta = GET_WHEEL_DELTA_WPARAM(wparam) > 0 ? 1 : -1;
                m_state.m_callBackFunc(MouseScrollEvent(0, delta));
            } break;
        default:
            break;
        }
    }

    Window* Window::Create(const WindowState& state) {
        OpenglContext::CreateOld();
        return new WindowsWindow(state);
    }

    WindowsWindow::WindowsWindow(const WindowState& state)
        : m_state(state), keys{}, mouse{}
    {
        registerWindowClass();
        createWindowsWindow();
        ShowWindow(m_hwnd, SW_NORMAL);
        UpdateWindow(m_hwnd);
    }

    void WindowsWindow::registerWindowClass() {
        HINSTANCE instance = GetModuleHandle(NULL);
        WNDCLASSEX wc{};
        if (GetClassInfoEx(instance, windowClassName, &wc))
            return;
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.lpfnWndProc = &WindowsWindow::WindowProc;
        wc.hInstance = instance;
        wc.lpszClassName = windowClassName;
        wc.style = CS_OWNDC;

        RegisterClassEx(&wc);
    }

    void WindowsWindow::createWindowsWindow() {
        m_hwnd = CreateWindow(
                windowClassName,
                m_state.title.c_str(),
                WS_OVERLAPPEDWINDOW,
                (GetSystemMetrics(SM_CXSCREEN) - m_state.width) / 2, (GetSystemMetrics(SM_CYSCREEN) - m_state.height) / 2,
                m_state.width, m_state.height,
                NULL,
                NULL,
                GetModuleHandle(NULL),
                this
                );

        m_hdc = GetDC(m_hwnd);
    }

    void WindowsWindow::setVSync(bool enable) { 
        m_state.isVSync = enable;
        wglSwapIntervalEXT(enable);
    }

    void WindowsWindow::update() {
        processMessage();
    }

    void WindowsWindow::showCursor(bool show) {
        ShowCursor(show);
    }

    void WindowsWindow::setCursorPos(const u32 x, const u32 y) {
        SetCursorPos(m_state.x + x, m_state.y + y);
    }

    void WindowsWindow::setFullScreen(bool enable) {
        i32 colourBits       = GetDeviceCaps(m_hdc, BITSPIXEL);
        i32 refreshRate      = GetDeviceCaps(m_hdc, VREFRESH);
        if (enable) {
            i32 fullscreenWidth  = GetDeviceCaps(m_hdc, DESKTOPHORZRES);
            i32 fullscreenHeight = GetDeviceCaps(m_hdc, DESKTOPVERTRES);
            DEVMODE fullscreenSettings;
            [[maybe_unused]]bool isChangeSuccessful;

            EnumDisplaySettings(NULL, 0, &fullscreenSettings);
            fullscreenSettings.dmPelsWidth        = fullscreenWidth;
            fullscreenSettings.dmPelsHeight       = fullscreenHeight;
            fullscreenSettings.dmBitsPerPel       = colourBits;
            fullscreenSettings.dmDisplayFrequency = refreshRate;
            fullscreenSettings.dmFields           = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;

            SetWindowLongPtr(m_hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
            SetWindowLongPtr(m_hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
                
            SetWindowPos(m_hwnd, HWND_TOP, 0, 0, fullscreenWidth, fullscreenHeight, SWP_SHOWWINDOW);
            isChangeSuccessful = ChangeDisplaySettings(&fullscreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;
        }
        else {
            DEVMODE fullscreenSettings;
            [[maybe_unused]]bool isChangeSuccessful;

            EnumDisplaySettings(NULL, 0, &fullscreenSettings);
            fullscreenSettings.dmPelsWidth        = 960;
            fullscreenSettings.dmPelsHeight       = 640;
            fullscreenSettings.dmBitsPerPel       = colourBits;
            fullscreenSettings.dmDisplayFrequency = refreshRate;
            fullscreenSettings.dmFields           = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;
            SetWindowLongPtr(m_hwnd, GWL_EXSTYLE, WS_EX_WINDOWEDGE);
            SetWindowLongPtr(m_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
            setSize(960, 640);
            setPosition((GetSystemMetrics(SM_CXSCREEN) - m_state.width) / 2, (GetSystemMetrics(SM_CYSCREEN) - m_state.height) / 2);
            isChangeSuccessful = ChangeDisplaySettings(&fullscreenSettings, CDS_SET_PRIMARY) == DISP_CHANGE_SUCCESSFUL;
        }
    }

    void WindowsWindow::setPosition(i32 x, i32 y) {
        SetWindowPos(m_hwnd, HWND_TOP, x, y, m_state.width, m_state.height, SWP_SHOWWINDOW);
    }

    void WindowsWindow::setSize(i32 width, i32 height) {
        SetWindowPos(m_hwnd, HWND_TOP, m_state.x, m_state.y, width, height, SWP_SHOWWINDOW);
    }

    void WindowsWindow::processMessage() {
        MSG msg{};
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    WindowsWindow::~WindowsWindow() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        if (!m_hglrc) {
            wglDeleteContext(m_hglrc);
            m_hglrc = nullptr;
        }
        if (!m_hdc) {
            ReleaseDC(m_hwnd, m_hdc);
            m_hdc = nullptr;
        }
        if (!m_hwnd) {
            DestroyWindow(m_hwnd);
            m_hwnd = nullptr;
        }
    }

}
