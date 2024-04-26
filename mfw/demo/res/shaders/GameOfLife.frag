#version 330 core

layout(location = 0) out vec4 frag_color;

uniform sampler2D screen;
in vec2 texCoords;

float gameOfLife() {
    vec2 size = textureSize(screen, 0);
    vec2 perpixel = 1.0f / size;

    float sum = 0;
    float c = texture(screen, texCoords).b;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            sum += texture(screen, texCoords + vec2(i, j) * perpixel).b;
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

    return c;
}

float smoothLife() {
    vec2 size = textureSize(screen, 0);
    vec2 perpixel = 1.0f / size;

    float ik = 0, ok = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            ik += texture(screen, texCoords + vec2(i, j) * perpixel).b;
        }
    }

    for (int i = -8; i <= 8; i++) {
        for (int j = -8; j <= 8; j++) {
            ok += texture(screen, texCoords + vec2(i, j) * perpixel).b;
        }
    }

    ok -= ik;
    float uo = ok / (289 - 9);
    float ui = ik / (9);

    if (ui >= 0.5 && uo >= 0.26 && uo <= 0.46) {
        return 1;
    }
    else if (ui < 0.5 && uo >= 0.27 && uo <= 0.36) {
        return 1;
    }
    return 0;
}

void main() {
    // frag_color = vec4(vec3(gameOfLife()), 1);
    frag_color = vec4(vec3(smoothLife()), 1);
}
