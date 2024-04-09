#version 330 core

layout(location = 0) out vec4 frag_color;

uniform sampler2D screen;
in vec2 texCoords;

void main() {
    vec2 size = textureSize(screen, 0);
    vec2 perpixel = 1.0f / size;

    float sum = 0;
    float c = texture(screen, texCoords).r;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            sum += texture(screen, texCoords + vec2(i, j) * perpixel).r;
        }
    }

    sum -= c;

    vec3 color;
    if (c == 1) {
        if (sum <= 1.0f || sum >= 4.0f) {
            c = 0;
        }
    }
    else {
        if (sum >= 3 && sum <= 3) {
            c = 1;
        }
    }

    frag_color = vec4(vec3(c), 1);
}
