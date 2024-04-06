#version 330 core

layout(location = 0) out vec4 frag_color;

#define dd 0.02

uniform sampler2D screen;
in vec2 texCoords;

void main() {
    vec2 texc = texCoords;
    vec2 size = textureSize(screen, 0);
    vec2 psize = 1.0 / size;

    vec3 color = vec3(0);

    color += vec3(texture(screen, texc + vec2(psize.x, 0)));
    color += vec3(texture(screen, texc - vec2(psize.x, 0)));
    color += vec3(texture(screen, texc + vec2(0, psize.y)));
    color += vec3(texture(screen, texc - vec2(0, psize.y)));

    color += vec3(texture(screen, texc + psize));
    color += vec3(texture(screen, texc - psize));
    color += vec3(texture(screen, texc + vec2(-psize.x, psize.y)));
    color += vec3(texture(screen, texc + vec2(psize.x, -psize.y)));

    color /= 8;

    color -= dd * length(color);

    frag_color = vec4(color, 1);
}
