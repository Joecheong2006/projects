#pragma once

#include "Window.h"
#include "InputEvent.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace  mfw {
    class WindowsWindow : public Window {
        friend class WindowsOpenglContext;
        friend class WindowsWindowInput;
    public:
        explicit WindowsWindow(const WindowState& state);
        ~WindowsWindow();

        virtual void update() override;
        inline virtual bool isRunning() const override { return m_state.isRunning; }
        inline virtual i32 width() const override { return m_state.width; }
        inline virtual i32 height() const override { return m_state.height; }
        inline virtual i32 x() const override { return m_state.x; }
        inline virtual i32 y() const override { return m_state.y; }
        inline virtual const wchar_t* title() const override { return m_state.title.c_str(); }
        inline virtual void setEventCallBack(const std::function<void(const Event&)>& callBackFunction) override {
            m_state.m_callBackFunc = std::move(callBackFunction);
        }
        inline virtual void setVSync(bool enable) override;
        inline virtual void* getNativeWindow() override { return this; }
        inline virtual void close() override { m_state.isRunning = false; }
        virtual void showCursor(bool show = true) override;
        virtual void setCursorPos(u32 x, u32 y) override;
        inline virtual void swapBuffers() override { SwapBuffers(m_hdc); }
        virtual void setFullScreen(bool enable) override;
        virtual void setPosition(i32 x, i32 y) override;
        inline virtual void* getHandle() override { return m_hwnd; }
        virtual void setSize(i32 width, i32 height) override;
        virtual void setMode(WindowMode mode) override;
        virtual void setFocus() override;

    private:
        WindowState m_state;

        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
        void handleMessage(const UINT& message, const WPARAM& wparam, const LPARAM& lparam);
        void registerWindowClass();
        void createWindowsWindow();
        void processMessage();

        HDC m_hdc;
        HWND m_hwnd;
        HGLRC m_hglrc = nullptr;

        bool keys[520];

        struct Mouse {
            i32 x, y;
            bool show_cursor;
            bool buttons[8];
            KeyMode actions[8];
        } mouse;

    };
}

