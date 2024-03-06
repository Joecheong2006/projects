#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 tex_coord;

uniform mat4 view;
out vec2 texture_coord;

void main()
{
    vec4 pos = view * vec4(position.xy, 0, 1);
    texture_coord = tex_coord;
    gl_Position = pos;
};
