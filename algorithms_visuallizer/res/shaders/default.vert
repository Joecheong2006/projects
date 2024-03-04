#version 330 core

layout(location = 0) in vec2 position;

uniform mat4 model;

void main()
{
    vec4 pos = model * vec4(position, 0, 1);
    gl_Position = pos;
};
