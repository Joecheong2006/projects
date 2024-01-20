#include "Application.h"

#include "Input.h"
#include "Clock.h"
#include "OpenglContext.h"
#include "WindowsWindow.h"

namespace mfw {
    Application* Application::Instance = CreateApplication();

    Application::Application()
    {
        {
            START_CLOCK_DURATION("INIT WINDOW");
            m_window = Window::Create({"demo", 960, 640});
        }
        {
            START_CLOCK_DURATION("INIT OPENGL");
            OpenglContext::CreateMorden(m_window);
        }
        m_window->setEventCallBack([this](const Event& event) {
                    this->Eventhandle(event);
                });

        eventListener.addEventFunc<WindowCloseEvent>([](const Event& event) {
                    LOG_INFO("{}\n", event);
                });
        eventListener.addEventFunc<WindowMoveEvent>([](const Event& event) {
                    LOG_INFO("{}\n", event);
                });
        eventListener.addEventFunc<CursorMoveEvent>([](const Event& event) {
                    LOG_INFO("{}\n", event);
                });
        eventListener.addEventFunc<KeyEvent>([this](const Event& event) {
                    this->InputHandle(static_cast<const KeyEvent&>(event));
                });
        eventListener.addEventFunc<MouseButtonEvent>([this](const Event& event) {
                    this->MouseButtonHandle(static_cast<const MouseButtonEvent&>(event));
                });
        eventListener.addEventFunc<MouseScrollEvent>([this](const Event& event) {
                    this->MouseScrollHandle(static_cast<const MouseScrollEvent&>(event));
                });
    }

    Application::~Application() {
        delete m_window;
    }

    void Application::run() {
        glClearColor(0.1, 0.1, 0.1, 1);
        while (m_window->isRunning()) {
            glViewport(0, 0, m_window->width(), m_window->height());
            glClear(GL_COLOR_BUFFER_BIT);
            if (Input::KeyPress(' ')) {
                m_window->setCursorPos(m_window->width() * 0.5, m_window->height() * 0.5);
            }
            m_window->update();
            m_window->swapBuffers();
        }
    }

    void Application::Eventhandle(const Event& event) {
        eventListener.listen<WindowCloseEvent>(event);
        eventListener.listen<WindowMoveEvent>(event);
        eventListener.listen<CursorMoveEvent>(event);
        eventListener.listen<MouseButtonEvent>(event);
        eventListener.listen<MouseScrollEvent>(event);
        eventListener.listen<KeyEvent>(event);
    }

    void Application::InputHandle(const KeyEvent& event) {
        LOG_INFO("{}\n", (Event&)event);
        if (event.key == VK_ESCAPE && event.mode == KeyMode::Down) {
            m_window->close();
        }
    }

    void Application::MouseButtonHandle(const MouseButtonEvent& event) {
        if (event.button == MouseButton::Left) {
            LOG_INFO("left button {}\n", (i32)event.mode);
        }
    }

    void Application::MouseScrollHandle(const MouseScrollEvent& event) {
        LOG_INFO("ydelta: {}\n", event.ydelta);
    }

    void Application::Update() {
        m_window->swapBuffers();
    }
}

