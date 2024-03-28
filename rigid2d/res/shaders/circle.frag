#version 330 core

layout(location = 0) out vec4 frag_color;

uniform vec4 color;
in vec2 texture_coord;

void main()
{
    vec2 uv = (texture_coord) * 2 - 1;
    float a = smoothstep(0, 0.07, 1 - length(uv));
    frag_color = vec4(1, 1, 1, a) * color;
};
