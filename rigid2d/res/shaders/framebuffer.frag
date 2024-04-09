#version 330 core

layout(location = 0) out vec4 frag_color;

uniform sampler2D screen;
in vec2 texCoords;

void main() {
    frag_color = texture(screen, texCoords);
}
