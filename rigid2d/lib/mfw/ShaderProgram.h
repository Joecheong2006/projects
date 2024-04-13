#include "mfwpch.h"
#pragma once

#include "util.h"
#include "glm/glm.hpp"

namespace mfw {
    class ShaderProgram
    {
    private:
        std::unordered_map<std::string, i32> m_uniform_location_cache;
        u32 m_id;

    public:
        ShaderProgram();
        ~ShaderProgram();

        void create();
        void attachShader(u32 type, const char* path);
        void link();
        void bind() const;
        void unbind() const;
        void release();
        void set4f(const char* name, f32 v0, f32 v1, f32 v2, f32 v3);
        void set4f(const char* name, const glm::vec4& v);
        void set3f(const char* name, f32 v0, f32 v1, f32 v2);
        void set3f(const char* name, const glm::vec3& v);
        void set3f(const char* name, f32* v);
        void set2f(const char* name, f32 v0, f32 v1);
        void set1i(const char* name, i32 v0);
        void set1f(const char* name, f32 v0);

        void setMat4(const char* name, const glm::mat4& m);

    private:
        u32 compileShader(const char* source, u32 type);
        const std::string shaderSource(const char* path);
        i32 uniformLocation(const char* name);

    };
}

