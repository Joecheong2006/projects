#include <mfw.h>
#include "Circle.h"
#include "glm/gtc/matrix_transform.hpp"

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

using namespace mfw;
class DemoSandBox : public Application {
private:
    f32 zoom = 1;
    f32 offset_x = 0, offset_y = 0;
    glm::mat4 o;
    VertexArray vao;
    IndexBuffer ibo;
    VertexBuffer vbo;
    ShaderProgram shader;
    Circle::Manager cm;

public:
    DemoSandBox()
        : ibo(index, 6), vbo(vertex, sizeof(vertex))
    {
        VertexBufferLayout layout;
        layout.add<f32>(2);
        vao.applyBufferLayout(layout);
        shader.attachShader(GL_VERTEX_SHADER, "res/shaders/test.vert");
        shader.attachShader(GL_FRAGMENT_SHADER, "res/shaders/test.frag");
        shader.link();

        glClearColor(0.1, 0.1, 0.1, 0);

        cm.createCircle(glm::vec2(0), glm::vec3(1), 10);
        auto window = GetWindow();
        glm::vec2 ws = glm::vec2(window->width(), window->height()) / 20.f;
        o = glm::ortho(-ws.x, ws.x, -ws.y, ws.y, -1.0f, 1.0f);
    }

    virtual void Update() override {
        f32 frame = 1.0 / 144;

        auto window = GetWindow();
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        shader.bind();
        vao.bind();

        f32 width = window->width();
        f32 height = window->height();
        shader.set2f("resolution", width, height);
        shader.set2f("offset", offset_x, offset_y);
        shader.set1f("time", Time::GetCurrent());
        shader.set1f("zoom", zoom);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        cm.renderCircle(o);

        if (Input::KeyPress(' ')) {
            window->setCursorPos(window->width() * 0.5, window->height() * 0.5);
        }
        if (Input::KeyPress('W')) {
            offset_y += frame;
        }
        if (Input::KeyPress('A')) {
            offset_x -= frame;
        }
        if (Input::KeyPress('S')) {
            offset_y -= frame;
        }
        if (Input::KeyPress('D')) {
            offset_x += frame;
        }

        ImGui::Begin("status");
        ImGui::Text("hello, world\n");
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    virtual void OnInputKey(const KeyEvent& event) override {
        if (event.key == VK_ESCAPE && event.mode == KeyMode::Down) {
            Terminate();
        }
    }

    virtual void OnMouseScroll(const MouseScrollEvent& event) override {
        zoom -= event.ydelta * 0.1;
    }

    virtual void OnWindowResize(const WindowResizeEvent& event) override {
        glViewport(0, 0, event.width, event.height);
        glm::vec2 ws = glm::vec2(event.width, event.height) / 20.f;
        o = glm::ortho(-ws.x, ws.x, -ws.y, ws.y, -1.0f, 1.0f);
    }

    ~DemoSandBox() {
    }

};

mfw::Application* mfw::CreateApplication() {
    return new DemoSandBox();
}
