#include <mfw.h>

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
    VertexArray vao;
    IndexBuffer ibo;
    VertexBuffer vbo;
    ShaderProgram shader;
    f32 zoom = 1;
    f32 offset_x = 0, offset_y = 0;

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
        vao.unbind();
        ibo.unbind();
        vbo.unbind();
        shader.unbind();

        glClearColor(0.1, 0.22, 0.1, 1);
    }

    virtual void Update() override {
        auto window = GetWindow();
        glViewport(0, 0, window->width(), window->height());
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        vao.bind();
        shader.bind();

        f32 width = window->width();
        f32 height = window->height();
        shader.set2f("resolution", width, height);
        shader.set2f("offset", offset_x, offset_y);
        shader.set1f("time", Time::GetCurrent());
        shader.set1f("zoom", zoom);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        f32 frame = 1.0 / 144;

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
        LOG_EVENT_INFO(event);
    }

    ~DemoSandBox() {
    }

};

mfw::Application* mfw::CreateApplication() {
    return new DemoSandBox();
}
