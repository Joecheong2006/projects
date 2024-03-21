#version 330 core

layout(location = 0) out vec4 frag_color;

uniform vec3 color;
in vec2 texture_coord;

void main()
{
    vec2 uv = (texture_coord) * 2 - 1;
    frag_color = vec4(color, smoothstep(0, 0.095, 1 - length(uv)));
};
