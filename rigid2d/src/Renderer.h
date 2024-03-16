#pragma once

#include "util.h"
#include "logger.h"
#include "glm/glm.hpp"

#include "VertexArray.h"
#include "ShaderProgram.h"
#include "VertexBuffer.h"
#include "Texture2D.h"

#define COLOR(val) ((u32)val >> 16) / 255.0, (((u32)val << 16) >> 24) / 255.0, (((u32)val << 24) >> 24) / 255.0

class Circle;
namespace mfw {
    void GLClearError();
    bool GLLogCall(const char* file, i32 line, const char* func);

    class Renderer {
    public:
        Renderer();
        ~Renderer();

        void clear();
        void renderRactangle(const glm::mat4& proj, const glm::vec2& p1, const glm::vec2& p2, glm::vec3 color, f32 w);
        void renderCircle(const glm::mat4& proj, const Circle& circle);
        void renderCircle(const glm::mat4& proj, const Circle* circle);
        void renderImage(const glm::mat4& proj, const glm::vec2& p1, const glm::vec2& p2);

    };

    class ImageRenderer {
    public:
        VertexArray m_vao;
        ShaderProgram m_shader;
        VertexBuffer m_vbo;
        Texture2D m_texture;
        ImageRenderer(const char* path);

    };

}

#if defined(DEBUG) || defined(_DEBUG)
#define GLCALL(x) mfw::GLClearError(); \
    x;\
    ASSERT(mfw::GLLogCall(__FILE__, __LINE__, #x))
#else
#define GLCALL(x)
#endif

