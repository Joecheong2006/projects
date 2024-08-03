#shader vertex
#version 430 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_tex_coord;

uniform mat4 view;

out vec2 tex_coord;

void main()
{
    gl_Position = view * vec4(a_pos.xyz, 1);
    tex_coord = a_tex_coord;
}

#shader fragment
#version 430 core

layout(location = 0) out vec4 frag_color;

uniform vec4 color;

uniform sampler2D tex;
uniform vec2 per_sprite;
uniform vec2 sprite_index;

in vec2 tex_coord;

void main()
{
    vec4 tex_color = texture(tex, (tex_coord + sprite_index) * per_sprite);
    if (tex_color.a == 0 || color.a == 0) {
        discard;
    }
    frag_color = tex_color * color;
}
