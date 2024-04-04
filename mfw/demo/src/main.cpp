#include <mfw.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"
#include <list>

static f32 vertex[] = {
     0.0,  0.5,
    -0.5, -0.5,
     0.5, -0.5,

     0.0,  0.5,
    -0.5, -0.5,
     0.5, -0.5,
};

struct TriangleStack {
    f32 data[300];
    i32 count = 0;
};

struct QuadStack {
    f32 data[6000];
    i32 count = 0;
};

void drawTriangle(TriangleStack& stack, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3) {
    stack.data[stack.count + 0] = p1.x;
    stack.data[stack.count + 1] = p1.y; stack.data[stack.count + 2] = p2.x;
    stack.data[stack.count + 3] = p2.y;
    stack.data[stack.count + 4] = p3.x;
    stack.data[stack.count + 5] = p3.y;
    stack.count += 6;
}

void drawQuad(QuadStack& stack, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 p4) {
    stack.data[stack.count + 0] = p1.x;
    stack.data[stack.count + 1] = p1.y;
    stack.data[stack.count + 2] = p2.x;
    stack.data[stack.count + 3] = p2.y;
    stack.data[stack.count + 4] = p3.x;
    stack.data[stack.count + 5] = p3.y;

    stack.data[stack.count + 6] = p1.x;
    stack.data[stack.count + 7] = p1.y;
    stack.data[stack.count + 8] = p4.x;
    stack.data[stack.count + 9] = p4.y;
    stack.data[stack.count + 10] = p3.x;
    stack.data[stack.count + 11] = p3.y;
    stack.count += 12;
}

using namespace mfw;
class DemoSandBox : public Application {
private:
    VertexArray vao;
    VertexBuffer vbo;
    ShaderProgram shader;

    i32 drawCount;

    f32 zoom = 1;
    f32 offset_x = 0, offset_y = 0;

public:
    DemoSandBox()
        : Application("demo", 960, 640), vbo(vertex, 0, GL_DYNAMIC_DRAW)
    {
        VertexBufferLayout layout;
        layout.add<f32>(2);
        vao.applyBufferLayout(layout);
        shader.attachShader(GL_VERTEX_SHADER, "res/shaders/default.vert");
        shader.attachShader(GL_FRAGMENT_SHADER, "res/shaders/default.frag");
        shader.link();
        vao.unbind();
        vbo.unbind();
        shader.unbind();

        glClearColor(0.1, 0.1, 0.1, 1);
    }

    virtual void Start() override {
        GetWindow().setVSync(true);
    }

    virtual void Update() override {
        auto window = &GetWindow();
        glViewport(0, 0, window->width(), window->height());
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        vao.bind();
        shader.bind();

        shader.set3f("color", glm::vec3(1));

        f32 frame = 1.0 / 144;

        {
            static QuadStack stack;
            static std::list<glm::vec2> posSamples;
            const auto m = Input::GetMouse();
            const f32 aspect = (f32)GetWindow().width() / GetWindow().height();
            const glm::vec2 mouse = glm::vec2(m.first / (f32)GetWindow().width(), 1 - m.second / (f32)GetWindow().height()) * 2.0f - 1.0f;
            const glm::vec2 offset = glm::vec2(1 * aspect, 1);

            if (posSamples.size() == 60) {
                posSamples.pop_front();
            }
            posSamples.push_back(mouse);

            for (auto iter = posSamples.begin();;) {
                if (iter == posSamples.end())
                    break;
                const glm::vec2 p1 = *iter;
                ++iter;
                if (iter == posSamples.end())
                    break;
                const glm::vec2 p2 = *iter;
                const glm::vec2 normal = glm::normalize(p2 - p1);
                const glm::vec2 t1 = glm::cross(glm::vec3(normal, 0), glm::vec3(0, 0, 1.0)) * 0.1f;
                const glm::vec2 d = p2 - p1;
                glm::vec2 pp1 = p1 - t1, pp2 = p1 + t1;
                drawQuad(stack, pp1, pp2, p2 + t1 + d, p2 - t1 + d);
                // pp2 = p2 + t1 + d;
                // pp1 = p2 - t1 + d;
            }

            vbo.setBuffer(stack.data, stack.count * sizeof(f32));
            drawCount = stack.count * 0.5;
            glDrawArrays(GL_TRIANGLES, 0, drawCount);
            stack.count = 0;
        }

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

    bool fullScreen = false;
    virtual void OnInputKey(const KeyEvent& event) override {
        if (event.key == VK_ESCAPE && event.mode == KeyMode::Down) {
            Terminate();
        }
        auto main = &GetWindow();

        if (event.key == 'F' && event.mode == KeyMode::Down) {
            fullScreen = !fullScreen;
            main->setFullScreen(fullScreen);
            glViewport(0, 0, main->width(), main->height());
        }
    }

    virtual void OnMouseScroll(const MouseScrollEvent& event) override {
        zoom -= event.ydelta * 0.1;
    }

    virtual void OnCursorMove(const CursorMoveEvent& event) override { 
    }

    virtual void OnWindowResize(const WindowResizeEvent& event) override {
        glViewport(0, 0, event.width, event.height);
    }

    virtual void OnWindowFocus(const WindowFocusEvent& event) override {
        LOG_EVENT_INFO(event);
        if (fullScreen) {
            GetWindow().setMode(WindowMode::Maximize);
        }
    }

    virtual void OnWindowNotFocus(const WindowNotFocusEvent& event) override {
        LOG_EVENT_INFO(event);
        if (fullScreen) {
            GetWindow().setMode(WindowMode::Minimize);
        }
    }

    ~DemoSandBox() {
    }

};

mfw::Application* mfw::CreateApplication() {
    return new DemoSandBox();
}
