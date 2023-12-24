#pragma once

#include "EventSystem.h"
#include "Window.h"
#include "mfwpch.h"

#define MFW_DEFAULT_STYLE WS_OVERLAPPEDWINDOW
#define MFW_OVERLAPPED WS_OVERLAPPED
#define MFW_CAPTION WS_CAPTION
#define MFW_TITLEBAR WS_SYSMENU
#define MFW_RESIZEABLE WS_THICKFRAME
#define MFW_MINIMIZE WS_MINIMIZEBOX
#define MFW_MAXIMIZE WS_MAXIMIZEBOX

namespace  mfw {
    class WindowsWindow : public Window {
        friend class OpenglContext;
    public:
        explicit WindowsWindow(const char* title, int width, int height, int style = MFW_DEFAULT_STYLE);
        ~WindowsWindow();

        virtual bool isRunning() const override { return m_state.isRunning; }
        virtual i32 width() const override { return m_state.width; }
        virtual i32 height() const override { return m_state.height; }
        virtual i32 x() const override { return m_state.x; }
        virtual i32 y() const override { return m_state.y; }
        virtual i32 style() const override { return m_state.style; }
        virtual const char* title() const override { return m_state.title.c_str(); }
        void setEventCallBack(const std::function<void(const Event&)>& callBackFunction) override {
            m_state.m_callBackFunction = std::move(callBackFunction);
        }
        virtual void setVSync(bool enable) override { m_state.isVSync = enable; }
        virtual void initialize() override;
        virtual void update() override;

        inline void swapBuffers() const { SwapBuffers(m_hdc); }

    private:
        WindowState m_state;

        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
        void handleMessage(UINT message, WPARAM wparam, LPARAM lparam);
        void registerWindowClass();
        void createWindowsWindow();
        void processMessage();

        HDC m_hdc;
        HWND m_hwnd;
        static const char* windowClassName;

    };
}

