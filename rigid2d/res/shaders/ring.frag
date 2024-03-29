#version 330 core

layout(location = 0) out vec4 frag_color;

uniform vec4 color;
uniform float width;
in vec2 texture_coord;

void main()
{
    vec2 uv = (texture_coord) * 2 - 1;
    float a = pow(smoothstep(0, width, abs((1.0f - width) * 1.03f - length(uv))), 2);
    frag_color = vec4(1, 1, 1, 1 - a) * color;
};
