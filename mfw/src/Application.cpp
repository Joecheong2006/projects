#include "Application.h"
#include "EventSystem.h"
#include "WindowEventSystem.h"
#include <mfwpch.h>

namespace mfw {
    Application::Application()
        : m_window()
    {
        m_window.initialize({MFW_DEFAULT_STYLE, "demo", 960, 640});
        m_window.setEventCallBack([this](const Event& event) {
                    this->handleEvent(event);
                });
        openglContext.createMorden(&m_window);

        eventListener.addEventFunc<WindowCloseEvent>([](const Event& event) {
                    LOG_INFOLN(event);
                });
        eventListener.addEventFunc<WindowMoveEvent>([](const Event& event) {
                    LOG_INFOLN(event);
                });
    }

    Application::~Application() {
        openglContext.release();
    }

    void Application::run() {
        glClearColor(0.1, 0.1, 0.1, 1);
        while (m_window.isRunning()) {
            glViewport(0, 0, m_window.width(), m_window.height());
            glClear(GL_COLOR_BUFFER_BIT);
            update();
            m_window.update();
        }
    }

    void Application::handleEvent(const Event& event) {
        eventListener.listen<WindowCloseEvent>(event);
        eventListener.listen<WindowMoveEvent>(event);
    }

    void Application::update() {
        m_window.swapBuffers();
    }
}

