#include "Application.h"

#include "Input.h"
#include "Clock.h"
#include "OpenglContext.h"
#include "Renderer.h"
#include "stb_image.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "ShaderProgram.h"
#include "Texture2D.h"

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
            GLCALL(glEnable(GL_BLEND));
            GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        }

        stbi_set_flip_vertically_on_load(true);

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
    }

    Application::~Application() {
        delete m_window;
    }

    void Application::run() {
        static f32 vertex[] = {
             1,  1,
            -1,  1,
             1, -1,
            -1, -1,
        };

        static u32 index[] = {
            1, 0, 2,
            1, 3, 2,
        };
        f32 zoom = 1;
        f32 offset_x = 0, offset_y = 0;

        float start = Time::GetCurrent(), end;
        i32 fps = 120;
        float frame = 1.0 / fps;
        VertexArray vao;
        IndexBuffer ibo(index, 6);
        VertexBuffer vbo(vertex, sizeof(vertex));
        ShaderProgram shader;
        VertexBufferLayout layout;
        layout.add<f32>(2);
        vao.applyBufferLayout(layout);
        shader.attachShader(GL_VERTEX_SHADER, "res/shaders/default.vert");
        shader.attachShader(GL_FRAGMENT_SHADER, "res/shaders/default.frag");
        //shader.attachShader(GL_VERTEX_SHADER, "res/shaders/test.vert");
        //shader.attachShader(GL_FRAGMENT_SHADER, "res/shaders/test.frag");
        shader.link();

        while (m_window->isRunning()) {
            glClear(GL_COLOR_BUFFER_BIT);

            vao.bind();
            shader.bind();

            f32 width = m_window->width();
            f32 height = m_window->height();
            shader.set2f("resolution", width, height);
            shader.set2f("offset", offset_x, offset_y);
            shader.set1f("time", Time::GetCurrent());
            shader.set1f("zoom", zoom);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            Update();

            m_window->update();
            m_window->swapBuffers();
            end = Time::GetCurrent();
            float cframe = end - start;
            if (cframe < frame) {
                Time::Sleep((frame - cframe) * 1000);
                end = Time::GetCurrent();
            }
            start = end;
        }
    }

    void Application::Eventhandle(const Event& event) {
        eventListener.listen<KeyEvent>(event);
        eventListener.listen<WindowResizeEvent>(event);
        eventListener.listen<WindowCloseEvent>(event);
        eventListener.listen<MouseButtonEvent>(event);
        eventListener.listen<MouseScrollEvent>(event);
        eventListener.listen<CursorMoveEvent>(event);
    }

}

