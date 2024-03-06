#version 330 core

layout(location = 0) out vec4 frag_color;

uniform vec3 color;
in vec2 texture_coord;

void main()
{
    float r = 1 - smoothstep(0.85, 1, length(texture_coord * 2 - 1));
    frag_color = vec4(color, r);
};
