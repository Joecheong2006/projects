#version 330 core

layout(location = 0) out vec4 frag_color;

uniform sampler2D tex;
uniform vec4 color;
in vec2 texture_coord;

void main()
{
    vec4 c = vec4(texture(tex, texture_coord));
    frag_color = vec4(1, 1, 1, c.a) * color;
};
