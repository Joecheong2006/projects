#pragma once
#include "Test.h"

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

class ScreenBufferTest : public Test {
public:
    ScreenBufferTest();
    ~ScreenBufferTest();
    virtual void Start() override;
    virtual void Update() override;
    virtual void UpdateImgui() override;

private:
    f32 frame, dd = 0.01, cursorSize = 1;
    i32 step = 10;
    bool stop = false;
    ScreenBuffer* screenBuffer;

    mfw::VertexArray vao;
    mfw::VertexBuffer vbo;
    mfw::ShaderProgram shader;

    glm::vec2 resolution = glm::vec2(720, 540);

};

