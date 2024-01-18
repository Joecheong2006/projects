#include "Application.h"
#include "EventSystem.h"
#include "WindowEventSystem.h"
#include "Clock.h"
#include <mfwpch.h>

namespace mfw {
    Application::Application()
        : m_window()
    {
        {
            START_CLOCK_DURATION("OPENGL INIT");
            openglContext.createOld();
        }
        {
            START_CLOCK_DURATION("WINDOWSWINDOW INIT");
            m_window.initialize({MFW_DEFAULT_STYLE, "demo", 960, 640});
        }
        {
            START_CLOCK_DURATION("MORDEN OPENGL INIT");
            openglContext.createMorden(&m_window);
        }
        m_window.show();
        m_window.setEventCallBack([this](const Event& event) {
                    this->handleEvent(event);
                });

        eventListener.addEventFunc<WindowCloseEvent>([](const Event& event) {
                    LOG_INFO("{}\n", event);
                });
        eventListener.addEventFunc<WindowMoveEvent>([](const Event& event) {
                    LOG_INFO("{}\n", event);
                });
        eventListener.addEventFunc<WindowKeyEvent>([this](const Event& event) {
                    //LOG_INFO("{}\n", event);
                    this->input(static_cast<const WindowKeyEvent&>(event));
                });
    }

    Application::~Application() {
        openglContext.release();
    }

    void Application::run() {
        //while (m_window.isRunning()) {
        //    update();
        //    m_window.update();
        //}
        glClearColor(0.1, 0.1, 0.1, 1);
        while (m_window.isRunning()) {
            glViewport(0, 0, m_window.width(), m_window.height());
            glClear(GL_COLOR_BUFFER_BIT);
            update();
            m_window.update();
        }
    }

    void Application::input(const WindowKeyEvent& event) {
        if (event.key == VK_ESCAPE && event.mode == KeyMode::Down) {
            m_window.close();
        }
        if (event.mode == KeyMode::Down || event.mode == KeyMode::Repeat) {
            LOG_INFO("{}", (char)event.key);
        }
    }

    void Application::handleEvent(const Event& event) {
        eventListener.listen<WindowCloseEvent>(event);
        eventListener.listen<WindowMoveEvent>(event);
        eventListener.listen<WindowKeyEvent>(event);
    }

    void Application::update() {
        m_window.swapBuffers();
    }
}

