#include <mfw.h>

using namespace mfw;

static f32 vertex[] = {
     0.0,  0.5,
    -0.5, -0.5,
     0.5, -0.5,
};

class DemoSandBox : public Application {
private:
    VertexArray vao;
    VertexBuffer vbo;
    ShaderProgram shader;

public:
    DemoSandBox()
        : vbo(vertex, sizeof(vertex))
    {
        VertexBufferLayout layout;
        layout.add<f32>(2);
        vao.applyBufferLayout(layout);
        shader.attachShader(GL_VERTEX_SHADER, "res/shaders/default.vert");
        shader.attachShader(GL_FRAGMENT_SHADER, "res/shaders/default.frag");
        shader.link();
    }

    virtual void Update() override {
        vao.bind();
        shader.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    ~DemoSandBox() {
    }

};

mfw::Application* mfw::CreateApplication() {
    return new DemoSandBox();
}
