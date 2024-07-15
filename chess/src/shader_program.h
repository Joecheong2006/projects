#ifndef _SHADER_PROGRAM_
#define _SHADER_PROGRAM_
#include "string.h"
#include "error_type.h"

typedef struct {
    string vertex;
    string fragment;
} shader_program;

string parse_shader_type(const char* file_path, const char* type);
shader_program parse_shader(const char* file_path);

error_type shader_program_init(shader_program*);
void shader_program_free(shader_program*);

u32 compile_shader(const char* source, u32 type);
u32 create_shader(const char* vertex_shader, const char* fragment_shader);

#endif
