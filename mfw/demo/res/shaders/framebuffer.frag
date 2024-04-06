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

    color += texture(screen, texc);

    // color += texture(screen, texc + vec2(psize.x, 0));
    // color += texture(screen, texc - vec2(psize.x, 0));
    // color += texture(screen, texc + vec2(0, psize.y));
    // color += texture(screen, texc - vec2(0, psize.y));
    //
    // color += texture(screen, texc + psize);
    // color += texture(screen, texc - psize);
    // color += texture(screen, texc + vec2(-psize.x, psize.y));
    // color += texture(screen, texc + vec2(psize.x, -psize.y));
    // color /= 5;

    color -= dd;

    frag_color = vec4(color.xyz, 1);
}
