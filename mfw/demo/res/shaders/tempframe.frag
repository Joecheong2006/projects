#version 330 core

layout(location = 0) out vec4 frag_color;

#define dd 0.008

uniform sampler2D screen;
in vec2 texCoords;

void main() {
    vec2 texc = texCoords;
    vec2 size = textureSize(screen, 0);
    vec2 psize = 1.0 / size;

    vec4 color = vec4(0);

    color = texture(screen, texc);
    float n1 = length(vec3(texture(screen, texc + vec2(psize.x, 0))));
    float n2 = length(vec3(texture(screen, texc - vec2(psize.x, 0))));
    float n3 = length(vec3(texture(screen, texc + vec2(0, psize.y))));
    float n4 = length(vec3(texture(screen, texc - vec2(0, psize.y))));

    float n5 = length(vec3(texture(screen, texc + psize)));
    float n6 = length(vec3(texture(screen, texc - psize)));
    float n7 = length(vec3(texture(screen, texc + vec2(-psize.x, psize.y))));
    float n8 = length(vec3(texture(screen, texc + vec2(psize.x, -psize.y))));

    if (n1 * n2 * n3 * n4 > 0) {
        color = vec4(0);
    }
    if (n1 + n2 + n3 + n4 == 0) {
        color = vec4(1);
    }
    if (n1 * n2 * n4 > 0) {
        color = vec4(0);
    }
    if (n7 * n8 > 0) {
        color = vec4(0);
    }
    if (n5 * n6 > 0) {
        color = vec4(1);
    }
    if (n5 * n6 > 0) {
        color = vec4(0);
    }
    if (n7 * n3 > 0) {
        color = vec4(0);
    }

    // if (n2 * n4 > 0) {
    //     color = vec4(1);
    // }
    //
    // if (n7 * n8 > 0) {
    //     color = vec4(1);
    // }

    // color -= dd;

    frag_color = vec4(color.rgb, 1);
}
