#version 330 core

layout(location = 0) in vec2 position;

uniform mat4 view;

void main()
{
    vec4 pos = view * vec4(position.xy, 0, 1);
    gl_Position = pos;
};
