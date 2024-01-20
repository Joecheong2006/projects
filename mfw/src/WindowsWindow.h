#pragma once

#include "EventSystem.h"
#include "Window.h"
#include "Input.h"
#include <windows.h>
#include <mfwpch.h>

namespace  mfw {
    class WindowsWindow : public Window {
        friend class OpenglContext;
        friend class WindowsWindowInput;
    public:
        explicit WindowsWindow(const WindowState& state);
        ~WindowsWindow();

        inline virtual bool isRunning() const override { return m_state.isRunning; }
        inline virtual i32 width() const override { return m_state.width; }
        inline virtual i32 height() const override { return m_state.height; }
        inline virtual i32 x() const override { return m_state.x; }
        inline virtual i32 y() const override { return m_state.y; }
        inline virtual const char* title() const override { return m_state.title.c_str(); }
        inline virtual void setEventCallBack(const std::function<void(const Event&)>& callBackFunction) override {
            m_state.m_callBackFunc = std::move(callBackFunction);
        }
        inline virtual void setVSync(bool enable) override { m_state.isVSync = enable; }
        virtual void update() override;
        inline virtual void* GetNativeWindow() override;
        inline virtual void close() override { m_state.isRunning = false; }
        inline virtual void showCursor() override;
        inline virtual void hideCursor() override;
        virtual void setCursorPos(u32 x, u32 y) override;

        inline void swapBuffers() const { SwapBuffers(m_hdc); }

    private:
        WindowState m_state;

        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
        void handleMessage(const UINT& message, const WPARAM& wparam, const LPARAM& lparam);
        void registerWindowClass();
        void createWindowsWindow();
        void processMessage();

        HDC m_hdc;
        HWND m_hwnd;

        bool keys[256];

        struct Mouse {
            bool show_cursor;
            i32 ax, ay;
            i32 rx, ry;
            bool buttons[8];
            KeyMode actions[8];
        } mouse;

        static const char* windowClassName;

    };
}

