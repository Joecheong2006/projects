#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 tex_coord;

out vec2 texture_coord;

void main() {
    texture_coord = tex_coord;
    gl_Position = vec4(position, 0, 1);
};

