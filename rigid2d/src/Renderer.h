#pragma once

#include "util.h"

#include <mfw/VertexArray.h>
#include <mfw/ShaderProgram.h>
#include <mfw/VertexBuffer.h>
#include <mfw/Texture2D.h>

#define COLOR(val) ((u32)val >> 16) / 255.0, (((u32)val << 16) >> 24) / 255.0, (((u32)val << 24) >> 24) / 255.0

const glm::vec3 red = glm::vec3(COLOR(0xff0000));
const glm::vec3 green = glm::vec3(COLOR(0x00ff00));
const glm::vec3 blue = glm::vec3(COLOR(0x0000ff));

class Circle;
namespace mfw {
    void GLClearError();
    bool GLLogCall(const char* file, i32 line, const char* func);

    class Renderer {
    public:
        Renderer();
        ~Renderer();

        void clear();
        void renderLine(const mat4& proj, const vec2& p1, const vec2& p2, vec3 color, real w);
        void renderLine(const mat4& proj, const vec2& p1, const vec2& p2, vec4 color, real w);
        void renderLineI(const mat4& proj, const vec2& p1, const vec2& p2, vec3 color, real w);
        void renderLineI(const mat4& proj, const vec2& p1, const vec2& p2, vec4 color, real w);
        void renderCircle(const mat4& proj, const Circle& circle);
        void renderCircle(const mat4& proj, const Circle* circle);
        void renderCircle(const mat4& proj, const vec2& p, real radius, vec4 color);
        void renderCircleI(const mat4& proj, const Circle& circle);
        void renderCircleI(const mat4& proj, const Circle* circle);
        void renderCircleI(const mat4& proj, const vec2& p, real radius, vec4 color);
        void renderImage(const mat4& proj, const vec2& p1, const vec2& p2);
        void renderRing(const mat4& proj, const vec2& p, real radius, real width, vec4 color);
        void renderRingI(const mat4& proj, const vec2& p, real radius, vec4 color);

    };

    class ImageRenderer {
    public:
        VertexArray m_vao;
        ShaderProgram m_shader;
        VertexBuffer m_vbo;
        Texture2D m_texture;
        ImageRenderer(const char* path, i32 wrap, i32 filter);

    };

}

#if defined(DEBUG) || defined(_DEBUG)
#define GLCALL(x)\
    mfw::GLClearError();\
    x;\
    ASSERT(mfw::GLLogCall(__FILE__, __LINE__, #x))
#else
#define GLCALL(x) x
#endif

