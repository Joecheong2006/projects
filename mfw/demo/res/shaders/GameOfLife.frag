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

#define e 2.71828
#define dt 0.1

float singleGrowth(float value, float u, float k) {
    float x = value - u;
    k = k * k * 2;
    return 2 * pow(e, -x * x / k) - 1;
}

float calKernal(float w, vec2 perpixel) {
    float result = 0;
    float count = 0;
    for (float i = -w; i <= w; ++i) {
        for (float j = -w; j <= w; ++j) {
            float r = length(vec2(i, j)) / w;
            float k = pow(4.0 * r * (1.0 - r), 1);
            result += texture(screen, texCoords + vec2(i, j) * perpixel).b * k;
            count += k;
        }
    }
    return result / count;
}

vec3 smoothLife() {
    vec2 size = textureSize(screen, 0);
    vec2 perpixel = 1.0f / size;
    float kernal = calKernal(20.0, perpixel);
    float c = clamp(singleGrowth(kernal, 0.2, 0.024) * dt + texture(screen, texCoords).b, 0.0, 1.0);
    return vec3(c);
}

void main() {
    frag_color = vec4(smoothLife(), 1);
}
