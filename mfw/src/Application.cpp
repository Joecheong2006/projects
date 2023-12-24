#include "Application.h"

#include "OpenglContext.h"
#include "WindowEventSystem.h"

namespace mfw {
    Application::Application()
        : m_window("demo", 960, 640, MFW_DEFAULT_STYLE)
    {
        m_window.setEventCallBack([this](const Event& event) {
                    this->handleEvent(event);
                });
        m_window.initialize();
        openglContext.createMorden(&m_window);
    }

    Application::~Application() {
        openglContext.release();
    }

    void Application::run() {
        while (m_window.isRunning()) {
            update();
            m_window.update();
        }
    }

    void Application::handleEvent(const Event& event) {
        EventDispatcher::Dispatch<WindowMoveEvent>(event);
    }

    void Application::update() {
        glViewport(0, 0, m_window.width(), m_window.height());
        glClearColor(0.1, 0.1, 0.1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        m_window.swapBuffers();
    }
}

