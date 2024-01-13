#pragma once

#include <iostream>
#include <unordered_map>
#include "util.h"
#include "glm/glm.hpp"


class ShaderProgram
{
private:
    std::unordered_map<std::string, i32> m_uniform_location_cache;
    u32 m_id;

public:
    ShaderProgram();
    ~ShaderProgram();

    void create();
    void attach_shader(u32 type, const std::string& path);
    void link();
    void bind() const;
    void unbind() const;
    void release();
    void set_4f(const std::string& name, f32 v0, f32 v1, f32 v2, f32 v3);
    void set_3f(const std::string& name, f32 v0, f32 v1, f32 v2);
    void set_3f(const std::string& name, f32* v);
    void set_2f(const std::string& name, f32 v0, f32 v1);
    void set_1i(const std::string& name, i32 v0);
    void set_1f(const std::string& name, f32 v0);

    void set_m4(const std::string& name, const glm::mat4& m);

private:
    u32 compile_shader(const std::string& source, u32 type);
    const std::string shader_source(const std::string& path);
    i32 uniform_location(const std::string& name);

};
