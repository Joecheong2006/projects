#pragma once
#include "Test.h"
#include "Layer.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "ShaderProgram.h"
#include "Texture2D.h"

struct ScreenBuffer {
    mfw::VertexArray vao;
    mfw::VertexBuffer vbo;
    mfw::ShaderProgram shader;

    u32 fbo, texture, rbo;
    ScreenBuffer(i32 width, i32 height);

};

class ScreenBufferTest : public mfw::Layer {
    friend class ScreenBufferUi;
public:
    ScreenBufferTest();
    ~ScreenBufferTest();
    virtual void OnStart() override;
    virtual void OnUpdate() override;

    struct kernal {
        float radius;
        float growth_u, growth_k;
    };

    kernal inner, outer;
    f32 dt;
    f32 frame, dd = 0.01, cursorSize = 1, cursorWidth;
    i32 step = 2;
    bool stop = false;
    glm::vec2 resolution = glm::vec2(720, 540);

private:
    ScreenBuffer* screenBuffer;
    mfw::VertexArray vao;
    mfw::VertexBuffer vbo;
    mfw::ShaderProgram shader;

};

