#include "Application.h"

#include "Input.h"
#include "Clock.h"
#include "OpenglContext.h"
#include "Renderer.h"
#include "stb_image/stb_image.h"
#include "InputEvent.h"
#include "WindowEvent.h"

namespace mfw {
    Application* Application::Instance = nullptr;

    Application::Application(): Application("app", 960, 640)
    {}

    Application::Application(const std::string& title, i32 width, i32 height)
    {
        ASSERT(Instance == nullptr);
        Instance = this;
        {
            START_CLOCK_TIMER("INIT WINDOW");
            m_window = std::unique_ptr<Window>(Window::Create({title, 100, 100, width, height}));
        }
        {
            START_CLOCK_TIMER("INIT OPENGL");
            OpenglContext::CreateMorden(m_window.get(), 3, 3);
            GLCALL(glEnable(GL_BLEND));
            GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        }

        stbi_set_flip_vertically_on_load(true);

        m_window->setVSync(true);

        m_window->setEventCallBack([this](const Event& event) {
                    Eventhandle(event);
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
        eventListener.addEventFunc<WindowResizeEvent>([this](const Event& event) {
                    WindowResize(static_cast<const WindowResizeEvent&>(event));
                });
        eventListener.addEventFunc<WindowCloseEvent>([this](const Event& event) {
                    WindowClose(static_cast<const WindowCloseEvent&>(event));
                });
        eventListener.addEventFunc<CursorMoveEvent>([this](const Event& event) {
                    CursorMove(static_cast<const CursorMoveEvent&>(event));
                });
        eventListener.addEventFunc<WindowFocusEvent>([this](const Event& event) {
                    WindowFocus(static_cast<const WindowFocusEvent&>(event));
                });
        eventListener.addEventFunc<WindowNotFocusEvent>([this](const Event& event) {
                    WindowNotFocus(static_cast<const WindowNotFocusEvent&>(event));
                });
    }

    Application::~Application() {
        OpenglContext::Release();
        delete Input::Instance;
        delete Time::Instance;
        delete OpenglContext::Instance;
    }

    void Application::run() {
        Start();
        while (m_window->isRunning()) {
            m_window->update();
            Update();
            m_window->swapBuffers();
        }
    }

    void Application::Eventhandle(const Event& event) {
        eventListener.listen<KeyEvent>(event);
        eventListener.listen<WindowResizeEvent>(event);
        eventListener.listen<WindowCloseEvent>(event);
        eventListener.listen<MouseButtonEvent>(event);
        eventListener.listen<MouseScrollEvent>(event);
        eventListener.listen<CursorMoveEvent>(event);
        eventListener.listen<WindowFocusEvent>(event);
        eventListener.listen<WindowNotFocusEvent>(event);
    }

}

