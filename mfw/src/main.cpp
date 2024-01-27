#include <mfw.h>
#include <thread>

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
    f32 zoom = 3.0;

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
        static i32 fps = 60;
        static f32 start = Time::GetCurrent();
        static f32 end = 0;

        auto window = GetWindow();
        glViewport(0, 0, window->width(), window->height());
        glClear(GL_COLOR_BUFFER_BIT);

        vao.bind();
        shader.bind();

        f32 width = window->width();
        f32 height = window->height();
        shader.set2f("resolution", width, height);
        shader.set1f("time", start);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        if (Input::KeyPress(' ')) {
            window->setCursorPos(window->width() * 0.5, window->height() * 0.5);
        }

        end = Time::GetCurrent();

        f32 duration = 1.0f / fps - end + start;
        if (duration > 0) {
            Time::Sleep(duration * 1000);
            end += duration;
        }
        start = end;
    }

    virtual void OnInputKey(const KeyEvent& event) override {
        if (event.key == VK_ESCAPE && event.mode == KeyMode::Down) {
            Terminate();
        }
    }

    ~DemoSandBox() {
    }

};

mfw::Application* mfw::CreateApplication() {
    return new DemoSandBox();
}
