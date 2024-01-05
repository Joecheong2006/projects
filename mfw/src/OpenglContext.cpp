#include "OpenglContext.h"
#include "mfwlog.h"
#include <typeinfo>

namespace mfw {
    const char* OpenglContextClassName = "__@@OpenglDummyWindow";
    void OpenglContext::createOld() {
        //HINSTANCE instance = GetModuleHandle(NULL);
        WNDCLASSEX wc{};
        //if (GetClassInfoEx(instance, OpenglContextClassName, &wc))
        //    return;
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.lpfnWndProc = &DefWindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = OpenglContextClassName;
        wc.style = CS_OWNDC;

        RegisterClassEx(&wc);
        HWND hwnd = CreateWindow(
                OpenglContextClassName,
                "",
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

        release();
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
    }

    void OpenglContext::createMorden(WindowsWindow* window) {
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
        wglChoosePixelFormatARB(window->m_hdc, attribList, nullptr, 1, &pixelFormat, &numFormats);

        PIXELFORMATDESCRIPTOR pfd;
        SetPixelFormat(window->m_hdc, pixelFormat, &pfd);

        const int contextAttribList[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            0
        };

        HGLRC shareHglrc = nullptr;
        m_hglrc = wglCreateContextAttribsARB(window->m_hdc, shareHglrc, contextAttribList);
        wglMakeContextCurrentARB(window->m_hdc, window->m_hdc, m_hglrc);

        LOG_INFO("OPENGL VERSION: {}\n", glGetString(GL_VERSION));
    }

    void OpenglContext::release() {
        wglDeleteContext(m_hglrc);
    }
}
