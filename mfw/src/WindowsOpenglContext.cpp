#include "WindowsOpenglContext.h"

#include "WindowsWindow.h"
#include "mfwlog.h"

namespace mfw {
    OpenglContext* OpenglContext::Instance = new WindowsOpenglContext();

    const wchar_t* OpenglContextClassName = L"__OpenglDummyWindow";

    void WindowsOpenglContext::CreateOldImpl() {
        HINSTANCE instance = GetModuleHandle(NULL);
        WNDCLASSEX wc{};
        if (GetClassInfoEx(instance, OpenglContextClassName, &wc))
            return;
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.lpfnWndProc = &DefWindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = OpenglContextClassName;
        wc.style = CS_OWNDC;

        RegisterClassEx(&wc);
        HWND hwnd = CreateWindow(
                OpenglContextClassName,
                L"",
                0,
                0, 0, 0, 0,
                NULL,
                NULL,
                GetModuleHandle(NULL),
                NULL
                );

        HDC hdc = GetDC(hwnd);

        PIXELFORMATDESCRIPTOR pfd = {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA,
            32,
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24,
            8,
            0,
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };

        int suggestPfdId = ChoosePixelFormat(hdc, &pfd);
        SetPixelFormat(hdc, suggestPfdId, &pfd);

        m_hglrc = wglCreateContext(hdc);
        wglMakeCurrent(hdc, m_hglrc);

        if (glewInit() != GLEW_OK) {
            LOG_INFO("GLEW INIT FAIL\n");
        }

        ReleaseImpl();
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
    }

    void WindowsOpenglContext::CreateMordenImpl(Window* window) {
        WindowsWindow* w = reinterpret_cast<WindowsWindow*>(window->getNativeWindow());

        const int attribList[] = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_STENCIL_BITS_ARB, 8,
            0,
        };

        int pixelFormat;
        unsigned int numFormats;
        wglChoosePixelFormatARB(w->m_hdc, attribList, nullptr, 1, &pixelFormat, &numFormats);

        PIXELFORMATDESCRIPTOR pfd;
        SetPixelFormat(w->m_hdc, pixelFormat, &pfd);

        const int contextAttribList[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            0
        };

        HGLRC shareHglrc = nullptr;
        w->m_hglrc = wglCreateContextAttribsARB(w->m_hdc, shareHglrc, contextAttribList);
        wglMakeContextCurrentARB(w->m_hdc, w->m_hdc, w->m_hglrc);

        LOG_INFO("OPENGL VERSION: {}\n", glGetString(GL_VERSION));
        //wglSwapIntervalEXT(1);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO io = ImGui::GetIO();
        ImGui::StyleColorsDark();
        ImGui_ImplWin32_InitForOpenGL(w->m_hwnd);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    void WindowsOpenglContext::ReleaseImpl() {
        wglDeleteContext(m_hglrc);
    }
}
