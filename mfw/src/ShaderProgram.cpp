#include "ShaderProgram.h"

#include <mfwpch.h>
#include "Renderer.h"
#include "glm/gtc/type_ptr.hpp"

namespace mfw {
    ShaderProgram::ShaderProgram()
    {
        GLCALL(m_id = glCreateProgram());
    }

    ShaderProgram::~ShaderProgram()
    {
        GLCALL(glDeleteProgram(m_id));
    }

    void ShaderProgram::attachShader(u32 type, const char* path)
    {
        u32 shader = compileShader(shaderSource(path).c_str(), type);
        ASSERT(shader != 0);
        GLCALL(glAttachShader(m_id, shader));
        GLCALL(glDeleteShader(shader));
    }

    void ShaderProgram::link()
    {
        GLCALL(glLinkProgram(m_id));
    }

    void ShaderProgram::bind() const
    {
        GLCALL(glUseProgram(m_id));
    }

    void ShaderProgram::unbind() const
    {
        GLCALL(glUseProgram(0));
    }

    void ShaderProgram::set4f(const char* name, f32 v0, f32 v1, f32 v2, f32 v3)
    {
        GLCALL(glUniform4f(uniformLocation(name), v0, v1, v2, v3));
    }

    void ShaderProgram::set3f(const char* name, f32 v0, f32 v1, f32 v2)
    {
        GLCALL(glUniform3f(uniformLocation(name), v0, v1, v2));
    }

    void ShaderProgram::set3f(const char* name, f32* v)
    {
        GLCALL(glUniform3f(uniformLocation(name), v[0], v[1], v[2]));
    }

    void ShaderProgram::set1i(const char* name, i32 v0)
    {
        GLCALL(glUniform1i(uniformLocation(name), v0));
    }

    void ShaderProgram::set1f(const char* name, f32 v0)
    {
        GLCALL(glUniform1f(uniformLocation(name), v0));
    }

    void ShaderProgram::setMat4(const char* name, const glm::mat4& m)
    {
        GLCALL(glUniformMatrix4fv(uniformLocation(name), 1, GL_FALSE, glm::value_ptr(m)));
    }

    u32 ShaderProgram::compileShader(const char* source, u32 type)
    {
        GLCALL(u32 id = glCreateShader(type));
        const char* src = source;
        GLCALL(glShaderSource(id, 1, &src, NULL));
        GLCALL(glCompileShader(id));

        int result;
        GLCALL(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
        if(result == GL_FALSE)
        {
            char infolog[512];
            GLCALL(glGetShaderInfoLog(id, 512, NULL, infolog));
            LOG_INFOLN("Compile error: ", infolog);
            GLCALL(glDeleteShader(id));
            return 0;
        }

        return id;
    }

    const std::string ShaderProgram::shaderSource(const char* path)
    {
        std::ifstream stream(path);
        std::string line;
        std::string source;
        while(getline(stream, line))
            source += line + "\n";
        return source;
    }

    i32 ShaderProgram::uniformLocation(const char* name)
    {
        if(m_uniform_location_cache.find(name) != m_uniform_location_cache.end())
            return m_uniform_location_cache[name];
        GLCALL(i32 location = glGetUniformLocation(m_id, name));
        if(location == -1)
            LOG_INFOLN("cannot found uniform locaiton: ", name);
        m_uniform_location_cache[name] = location;
        return location;
    }
}

