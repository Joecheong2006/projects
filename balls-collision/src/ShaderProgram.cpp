#include "ShaderProgram.hpp"
#include "Renderer.hpp"
#include <fstream>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

ShaderProgram::ShaderProgram()
{
    GLCALL(m_id = glCreateProgram());
}

ShaderProgram::~ShaderProgram()
{
    GLCALL(glDeleteProgram(m_id));
}

void ShaderProgram::attach_shader(u32 type, const std::string& path)
{
    u32 shader = compile_shader(shader_source(path), type);
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

void ShaderProgram::set_4f(const std::string& name, f32 v0, f32 v1, f32 v2, f32 v3)
{
    GLCALL(glUniform4f(uniform_location(name), v0, v1, v2, v3));
}

void ShaderProgram::set_3f(const std::string& name, f32 v0, f32 v1, f32 v2)
{
    GLCALL(glUniform3f(uniform_location(name), v0, v1, v2));
}

void ShaderProgram::set_3f(const std::string& name, glm::vec3 v)
{
    set_3f(name, glm::value_ptr(v));
}

void ShaderProgram::set_3f(const std::string& name, f32* v)
{
    GLCALL(glUniform3f(uniform_location(name), v[0], v[1], v[2]));
}

void ShaderProgram::set_2f(const std::string& name, f32 v0, f32 v1)
{
    GLCALL(glUniform2f(uniform_location(name), v0, v1));
}

void ShaderProgram::set_2f(const std::string& name, f32* v)
{
    GLCALL(glUniform2f(uniform_location(name), v[0], v[1]));
}

void ShaderProgram::set_1i(const std::string& name, i32 v0)
{
    GLCALL(glUniform1i(uniform_location(name), v0));
}

void ShaderProgram::set_1f(const std::string& name, f32 v0)
{
    GLCALL(glUniform1f(uniform_location(name), v0));
}

void ShaderProgram::set_m4(const std::string& name, const glm::mat4& m)
{
    GLCALL(glUniformMatrix4fv(uniform_location(name), 1, GL_FALSE, glm::value_ptr(m)));
}

u32 ShaderProgram::compile_shader(const std::string& source, u32 type)
{
    GLCALL(u32 id = glCreateShader(type));
    const char* src = source.c_str();
    GLCALL(glShaderSource(id, 1, &src, NULL));
    GLCALL(glCompileShader(id));

    int result;
    GLCALL(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if(result == GL_FALSE)
    {
        char infolog[512];
        GLCALL(glGetShaderInfoLog(id, 512, NULL, infolog));
        std::cout << "Compile shader error." << std::endl;
        std::cout << infolog << std::endl;
        GLCALL(glDeleteShader(id));
        return 0;
    }

    return id;
}

const std::string ShaderProgram::shader_source(const std::string& path)
{
    std::ifstream stream(path);
    std::string line;
    std::string source;
    while(getline(stream, line))
        source += line + "\n";
    return source;
}

i32 ShaderProgram::uniform_location(const std::string& name)
{
    if(m_uniform_location_cache.find(name) != m_uniform_location_cache.end())
        return m_uniform_location_cache[name];
    GLCALL(i32 location = glGetUniformLocation(m_id, name.c_str()));
    if(location == -1)
        std::cout << "cannot found uniform locaiton\n";
    m_uniform_location_cache[name] = location;
    return location;
}

