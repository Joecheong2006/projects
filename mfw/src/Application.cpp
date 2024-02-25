#include "Application.h"

#include "Input.h"
#include "Clock.h"
#include "OpenglContext.h"

namespace mfw {
    Application* Application::Instance = CreateApplication();

    Application::Application()
    {
        {
            START_CLOCK_TIMER("INIT WINDOW");
            m_window = Window::Create({"demo", 960, 640});
        }
        {
            START_CLOCK_TIMER("INIT OPENGL");
            OpenglContext::CreateMorden(m_window);
        }

        m_window->setEventCallBack([this](const Event& event) {
                    Eventhandle(event);
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
                    InputHandle(static_cast<const KeyEvent&>(event));
                });
        eventListener.addEventFunc<MouseButtonEvent>([this](const Event& event) {
                    MouseButtonHandle(static_cast<const MouseButtonEvent&>(event));
                });
        eventListener.addEventFunc<MouseScrollEvent>([this](const Event& event) {
                    MouseScrollHandle(static_cast<const MouseScrollEvent&>(event));
                });
    }

    Application::~Application() {
        delete m_window;
    }

    void Application::run() {
        float start = Time::GetCurrent(), end;
        i32 count = 0, fps = 120;
        float frame = 1.0 / fps;
        while (m_window->isRunning()) {
            ++count;
            if (count % fps == 0) {
                LOG_INFO("{}\n", count / fps);
            }
            Update();
            m_window->update();
            m_window->swapBuffers();
            end = Time::GetCurrent();
            float cframe = end - start;
            if (cframe < frame) {
                Time::Sleep((frame - cframe) * 1000);
                end = Time::GetCurrent();
                LOG_INFO("{}s\n", 1 / (end - start));
            }
            start = end;
        }
    }

    void Application::Eventhandle(const Event& event) {
        eventListener.listen<WindowCloseEvent>(event);
        eventListener.listen<WindowMoveEvent>(event);
        eventListener.listen<MouseButtonEvent>(event);
        eventListener.listen<MouseScrollEvent>(event);
        eventListener.listen<KeyEvent>(event);
    }

}

