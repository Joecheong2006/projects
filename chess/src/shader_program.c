#include "shader_program.h"
#include "util.h"
#include <stdio.h>
#include <string.h>
#include <glad/glad.h>

string parse_shader_type(const char* file_path, const char* type)
{
    ASSERT(file_path != NULL && type != NULL);
    string ret = make_string("");

    FILE* file = fopen(file_path, "r");
    if(file == NULL)
    {
        printf("can't open file");
        return ret;
    }

    char line[0xff];
    bool is_type = false;
    while(fgets(line, 0xff, file) != NULL)
    {
        if(strstr(line, "#shader"))
        {
            is_type = false;
            if(strstr(line, type))
            {
                is_type = true;
                continue;
            }
        }
        if(is_type) string_push(ret, line);
    }

    fclose(file);
    return ret;
}

shader_program parse_shader(const char* file_path)
{
    ASSERT(file_path != NULL);
    shader_program shader;
    shader.vertex = parse_shader_type(file_path, "vertex");
    shader.fragment = parse_shader_type(file_path, "fragment");
    return shader;
}

error_type shader_program_init(shader_program* self)
{
    ASSERT(self != NULL);
    self->vertex = make_string("");
    self->fragment = make_string("");
    return ErrorNone;
}

void shader_program_free(shader_program* self)
{
    ASSERT(self != NULL);
    free_string(self->vertex);
    free_string(self->fragment);
}

u32 compile_shader(const char* source, u32 type)
{
    ASSERT(source!= NULL);
    u32 id = glCreateShader(type);
    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);
    return id;
}

u32 create_shader(const char* vertex_shader, const char* fragment_shader)
{
    ASSERT(vertex_shader != NULL && fragment_shader);
    u32 program = glCreateProgram();
    u32 vs = compile_shader(vertex_shader, GL_VERTEX_SHADER);
    u32 fs = compile_shader(fragment_shader, GL_FRAGMENT_SHADER);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}
