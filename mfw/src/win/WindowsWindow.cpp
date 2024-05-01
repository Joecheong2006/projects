#include "win/WindowsWindow.h"

#include "WindowEvent.h"
#include "Input.h"
#include "OpenglContext.h"
#include "glad/wgl.h"
#include "KeyCode.h"

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
                i32 key = wparam;
                WORD flags = HIWORD(lparam);
                WORD scancode = LOBYTE(flags);
                if ((flags & KF_EXTENDED) == KF_EXTENDED) {
                    scancode = MAKEWORD(scancode, 0xe0);
                }
                BOOL keyRepeat = ((flags & KF_REPEAT) == KF_REPEAT) * 2;
                BOOL keyDown = !keyRepeat;
                BOOL keyRelease = ((flags & KF_UP) == KF_UP);
                KeyMode mode = static_cast<KeyMode>(keyRepeat + keyDown + keyRelease);

                key = Input::GetKeyCode(MapVirtualKeyW(scancode, MAPVK_VSC_TO_VK_EX));
                if (key != MF_KEY_NONE) {
                    keys[key] = mode == KeyMode::Down || mode == KeyMode::Press;
                    m_state.m_callBackFunc(KeyEvent(key, scancode, mode));
                }
            } break;

        case WM_XBUTTONUP:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONDOWN:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN: {
                int button;
                if (message == WM_LBUTTONDOWN || message == WM_LBUTTONUP) {
                    button = MF_MOUSE_BUTTON_LEFT;
                }
                else if (message == WM_RBUTTONDOWN || message == WM_RBUTTONUP) {
                    button = MF_MOUSE_BUTTON_RIGHT;
                }
                else if (message == WM_MBUTTONDOWN || message == WM_MBUTTONUP) {
                    button = MF_MOUSE_BUTTON_MIDDLE;
                }
                else if (GET_XBUTTON_WPARAM(wparam) == XBUTTON1) {
                    button = MF_MOUSE_BUTTON_4;
                }
                else {
                    button = MF_MOUSE_BUTTON_5;
                }
                if (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_XBUTTONDOWN) {
                    mouse.buttons[button] = true;
                    mouse.actions[button] = KeyMode::Down;
                    m_state.m_callBackFunc(MouseButtonEvent(button, KeyMode::Down));
                }
                else {
                    mouse.buttons[button] = false;
                    mouse.actions[button] = KeyMode::Release;
                    m_state.m_callBackFunc(MouseButtonEvent(button, KeyMode::Release));
                }
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
        RECT rt = {0, 0, m_state.width, m_state.height};
        AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, FALSE);
        m_state.width = rt.right - rt.left;
        m_state.height = rt.bottom - rt.top;

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

    // IT DOESN'T WORK FOR SOME REASOM ON SCHOOL COMPUTERS
    void WindowsWindow::setFullScreen(bool enable) {
        static WINDOWPLACEMENT windowPlacement;
        if (enable) {
            GetWindowPlacement(m_hwnd, &windowPlacement);
            i32 fullscreenWidth  = GetDeviceCaps(m_hdc, DESKTOPHORZRES);
            i32 fullscreenHeight = GetDeviceCaps(m_hdc, DESKTOPVERTRES);
            i32 colourBits       = GetDeviceCaps(m_hdc, BITSPIXEL);
            i32 refreshRate      = GetDeviceCaps(m_hdc, VREFRESH);
            DEVMODE fullscreenSettings;
            [[maybe_unused]] bool isChangeSuccessful;

            EnumDisplaySettings(NULL, 0, &fullscreenSettings);
            fullscreenSettings.dmPelsWidth        = fullscreenWidth;
            fullscreenSettings.dmPelsHeight       = fullscreenHeight;
            fullscreenSettings.dmBitsPerPel       = colourBits;
            fullscreenSettings.dmDisplayFrequency = refreshRate;
            fullscreenSettings.dmFields           = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;

            SetWindowLongPtr(m_hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
                
            SetWindowPos(m_hwnd, HWND_TOP, 0, 0, fullscreenWidth, fullscreenHeight, SWP_SHOWWINDOW);
            isChangeSuccessful = ChangeDisplaySettings(&fullscreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;
        }
        else {
            [[maybe_unused]]bool isChangeSuccessful;
            SetWindowLongPtr(m_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
            
            SetWindowPlacement(m_hwnd, &windowPlacement);
            SetWindowPos(m_hwnd, 0, 0, 0, 0, 0,
                    SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
            isChangeSuccessful = ChangeDisplaySettings(NULL, CDS_RESET) == DISP_CHANGE_SUCCESSFUL;
        }
    }

    void WindowsWindow::setPosition(i32 x, i32 y) {
        SetWindowPos(m_hwnd, HWND_TOP, x, y, m_state.width, m_state.height, SWP_SHOWWINDOW);
    }

    void WindowsWindow::setSize(i32 width, i32 height) {
        SetWindowPos(m_hwnd, HWND_TOP, m_state.x, m_state.y, width, height, SWP_SHOWWINDOW);
    }

    void WindowsWindow::setMode(WindowMode mode) {
        i32 correctMode;
        switch (mode) {
        case WindowMode::Show: correctMode = SW_SHOW; break;
        case WindowMode::Hide: correctMode = SW_HIDE; break;
        case WindowMode::Minimize: correctMode = SW_MINIMIZE; break;
        case WindowMode::Maximize: correctMode = SW_MAXIMIZE; break;
        case WindowMode::NoActive: correctMode = SW_SHOWNOACTIVATE; break;
        default: {
                 LOG_WARN("Invail WindowMode: {}", (i32)mode);
                 ASSERT(true);
            } break;
        }
        ShowWindow(m_hwnd, correctMode);
        UpdateWindow(m_hwnd);
    }

    void WindowsWindow::setFocus() {
        SetFocus(m_hwnd);
    }

    void WindowsWindow::processMessage() {
        MSG msg{};
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    WindowsWindow::~WindowsWindow() {
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
