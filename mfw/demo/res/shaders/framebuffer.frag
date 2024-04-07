#version 330 core

layout(location = 0) out vec4 frag_color;

uniform sampler2D screen;
uniform float dd;
in vec2 texCoords;

void main() {
    vec2 texc = texCoords;
    vec2 size = textureSize(screen, 0);
    vec2 psize = 1.0 / size;

    vec4 color = vec4(0);

    color += texture(screen, texc);

    color += texture(screen, texc + vec2(psize.x, 0));
    color += texture(screen, texc - vec2(psize.x, 0));
    color += texture(screen, texc + vec2(0, psize.y));
    color += texture(screen, texc - vec2(0, psize.y));

    color += texture(screen, texc + psize);
    color += texture(screen, texc - psize);
    color += texture(screen, texc + vec2(-psize.x, psize.y));
    color += texture(screen, texc + vec2(psize.x, -psize.y));
    color /= 9;

    color -= dd;

    if (length(color) < 0.01) {
        color += dd * 0.9;
    }
    else if (length(color) < 0.1) {
        color += dd * 0.8;
    }
    else if (length(color) < 0.3) {
        color += dd * 0.7;
    }

    frag_color = vec4(color.xyz, 1);
}
