#include "Application.h"

#include "Input.h"

namespace mfw {
    Application* Application::Instance = CreateApplication();

    Application::Application()
        : m_window(Window::Create({"demo", 960, 640}))
    {
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
        while (m_window->isRunning()) {
            if (Input::KeyPress(' ')) {
                m_window->setCursorPos(m_window->width() * 0.5, m_window->height() * 0.5);
            }
            Update();
            m_window->update();
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
    }
}

