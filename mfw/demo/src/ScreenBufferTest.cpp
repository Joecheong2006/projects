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
    resolution = glm::vec2(window->width(), window->height());
    screenBuffer = new ScreenBuffer(resolution.x, resolution.y);

    glViewport(0, 0, window->width(), window->height());
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void ScreenBufferTest::Update() {
    Timer timer;
    auto window = &Application::Get().GetWindow();
    glViewport(0, 0, resolution.x, resolution.y);

    glBindFramebuffer(GL_FRAMEBUFFER, screenBuffer->fbo);

    vao.bind();
    shader.bind();
    shader.set3f("color", color);

    const auto m = Input::GetMouse();
    const f32 aspect = (f32)window->width() / window->height();
    const glm::vec2 mouse = glm::vec2(m.first / (f32)window->width(), 1 - m.second / (f32)window->height()) * 2.0f - 1.0f;
    const glm::vec2 offset = glm::vec2(0.01, 0.01 * aspect) * cursorSize;

    vertex[0]  = mouse.x + offset.x;
    vertex[1]  = mouse.y + offset.y;
    vertex[2]  = mouse.x + offset.x;
    vertex[3]  = mouse.y - offset.y;
    vertex[4]  = mouse.x - offset.x;
    vertex[5]  = mouse.y - offset.y;

    vertex[6]  = mouse.x + offset.x;
    vertex[7]  = mouse.y + offset.y;
    vertex[8]  = mouse.x - offset.x;
    vertex[9]  = mouse.y + offset.y;
    vertex[10] = mouse.x - offset.x;
    vertex[11] = mouse.y - offset.y;

    vbo.setBuffer(vertex, sizeof(vertex));
    glDrawArrays(GL_TRIANGLES, 0, 6);

    screenBuffer->shader.bind();
    screenBuffer->shader.set1f("dd", dd);
    screenBuffer->vao.bind();
    glBindTexture(GL_TEXTURE_2D, screenBuffer->texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glViewport(0, 0, window->width(), window->height());
    screenBuffer->shader.bind();
    screenBuffer->shader.set1f("dd", dd);
    screenBuffer->vao.bind();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    frame = timer.getDuration();
}

void ScreenBufferTest::UpdateImgui() {
    ImGui::Text("frame: %gms", frame * 1000);
    ImGui::ColorPicker3("color", color);
    ImGui::SliderFloat("decreament", &dd, 0, 0.1);
    ImGui::SliderFloat("cursor size", &cursorSize, 1, 5);
    if (ImGui::SliderFloat2("resolution", glm::value_ptr(resolution), 120, 2560)) {
        delete screenBuffer;
        screenBuffer = new ScreenBuffer(resolution.x, resolution.y);
    }
}


