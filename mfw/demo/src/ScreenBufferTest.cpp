#include "ScreenbufferTest.h"

#include "Application.h"
#include "Input.h"
#include "Clock.h"
#include "Renderer.h"
#include "imgui/imgui.h"
#include "glm/gtc/type_ptr.hpp"

static f32 ract[] = {
     1.0,  1.0,   1.0, 1.0,
     1.0, -1.0,   1.0, 0.0,
    -1.0, -1.0,   0.0, 0.0,

     1.0,  1.0,   1.0, 1.0,
    -1.0,  1.0,   0.0, 1.0,
    -1.0, -1.0,   0.0, 0.0,
};

static f32 vertex[] = {
     0.01,  0.01,
     0.01, -0.01,
    -0.01, -0.01,

     0.01,  0.01,
    -0.01,  0.01,
    -0.01, -0.01,
};

using namespace mfw;

ScreenBuffer::ScreenBuffer(i32 width, i32 height): vbo(ract, sizeof(ract), GL_STATIC_DRAW)
{
    mfw::VertexBufferLayout layout;
    layout.add<f32>(2);
    layout.add<f32>(2);
    vao.applyBufferLayout(layout);

    shader.attachShader(GL_VERTEX_SHADER, "res/shaders/framebuffer.vert");
    shader.attachShader(GL_FRAGMENT_SHADER, "res/shaders/framebuffer.frag");
    shader.link();

    vao.unbind();
    vbo.unbind();
    shader.unbind();

    GLCALL(glGenFramebuffers(1, &fbo));
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, fbo));

    GLCALL(glGenTextures(1, &texture));
    GLCALL(glBindTexture(GL_TEXTURE_2D, texture));
    GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCALL(glBindTexture(GL_TEXTURE_2D, 0));

    GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0));

    GLCALL(glGenRenderbuffers(1, &rbo));
    GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, rbo));
    GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
    GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));

    GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo));

    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "error: " << status << '\n';
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ScreenBufferTest::ScreenBufferTest(): vbo(vertex, sizeof(vertex), GL_DYNAMIC_DRAW)
{}

ScreenBufferTest::~ScreenBufferTest() {
    if (screenBuffer) {
        delete screenBuffer;
    }
}

static f32 color[3] = {COLOR(0x977F89)};
void ScreenBufferTest::Start() {
    name = "ScreenBufferTest";

    VertexBufferLayout layout;
    layout.add<f32>(2);
    vao.applyBufferLayout(layout);
    shader.attachShader(GL_VERTEX_SHADER, "res/shaders/default.vert");
    shader.attachShader(GL_FRAGMENT_SHADER, "res/shaders/default.frag");
    shader.link();
    vao.unbind();
    vbo.unbind();
    shader.unbind();

    auto window = &Application::Get().GetWindow();
    window->setFullScreen(true);
    resolution = glm::vec2(320, 240);
    // resolution = glm::vec2(window->width(), window->height());
    screenBuffer = new ScreenBuffer(resolution.x, resolution.y);

    glViewport(0, 0, resolution.x, resolution.y);
    glClearColor(1, 1, 1, 1);
}

void ScreenBufferTest::Update() {
    Timer timer;
    auto window = &Application::Get().GetWindow();

    glBindFramebuffer(GL_FRAMEBUFFER, screenBuffer->fbo);
    if (Input::KeyPress('R')) {
        glClear(GL_COLOR_BUFFER_BIT);
    }
    glViewport(0, 0, resolution.x, resolution.y);

    if (Input::MouseButtonDown(MouseButton::Left)) {
        auto[x, y] = Input::GetMouse();
        const glm::vec2 mouse = glm::vec2(x / (f32)window->width(), 1 - y / (f32)window->height()) * 2.0f - 1.0f;
        vertex[0] = mouse.x;
        vertex[1] = mouse.y;

        vao.bind();
        shader.bind();
        color[0] = 1;
        color[1] = 1;
        color[2] = 1;
        shader.set3f("color", color);
        vbo.setBuffer(vertex, 2 * sizeof(f32));
        glDrawArrays(GL_POINTS, 0, 1);
    }

    static int c = 0;
    c++;
    if (c % step == 0) {
        u32 newTexture;
        GLCALL(glGenTextures(1, &newTexture));
        GLCALL(glBindTexture(GL_TEXTURE_2D, newTexture));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resolution.x, resolution.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
        GLCALL(glReadBuffer(GL_COLOR_ATTACHMENT0));
        GLCALL(glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, resolution.x, resolution.y));

        screenBuffer->vao.bind();
        ShaderProgram shader;
        shader.attachShader(GL_VERTEX_SHADER, "res/shaders/GameOfLive.vert");
        shader.attachShader(GL_FRAGMENT_SHADER, "res/shaders/GameOfLive.frag");
        shader.link();
        shader.bind();
        glBindTexture(GL_TEXTURE_2D, newTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDeleteTextures(1, &newTexture);
        c = 0;
    }

    screenBuffer->shader.bind();
    screenBuffer->vao.bind();
    glBindTexture(GL_TEXTURE_2D, screenBuffer->texture);
    glViewport(0, 0, window->width(), window->height());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    frame = timer.getDuration();
}

void ScreenBufferTest::UpdateImgui() {
    ImGui::Text("frame: %gms", frame * 1000);
    ImGui::ColorPicker3("color", color);
    ImGui::SliderInt("step", &step, 1, 40);
    if (ImGui::SliderFloat2("resolution", glm::value_ptr(resolution), 120, 2560)) {
        delete screenBuffer;
        screenBuffer = new ScreenBuffer(resolution.x, resolution.y);
    }
}


