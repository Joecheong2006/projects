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
#define pi 3.14159265

float sigmoid_a(float x, float a, float b) {
    return 1.0 / (1.0 + exp(-(x - a) * 4.0 / b));
}

float sigmoid_ab(float x, float a, float b, float ea, float eb) {
    return sigmoid_a(x, a, ea) * (1.0 - sigmoid_a(x, b, eb));
}

float sigmoid_mix(float x, float y, float m, float em) {
    return x * (1.0 - sigmoid_a(m, 0.5, em)) + y * sigmoid_a(m, 0.5, em);
}

float ramp_step(float value, float clip) {
    return clamp(clip - value + 0.5, 0, 1);
}

float singleGrowth(float value, float u, float k) {
    float x = value - u;
    k = k * k * 2.0;
    return 2.0 * exp(-x * x / k) - 1.0;
}

float calKernal(float w, vec2 perpixel) {
    float result = 0;
    for (float i = -w; i <= w; ++i) {
        for (float j = -w; j <= w; ++j) {
            float r = length(vec2(i, j));
            float weight = texture(screen, texCoords + vec2(i, j) * perpixel).b;
            result += weight * ramp_step(r, w);
        }
    }
    return result;
}

struct Kernal {
    float radius;
    float growth_u, growth_k;
};

uniform Kernal outer;
uniform Kernal inner;
uniform float dt;

vec3 smoothLife() {
    vec2 size = textureSize(screen, 0);
    vec2 perpixel = 1.0f / size;
    vec4 current = texture(screen, texCoords);

    float inner_area = (pi * inner.radius * inner.radius);
    float outer_area = (pi * outer.radius * outer.radius) - inner_area;

    float i = calKernal(inner.radius, perpixel) / inner_area;
    float k = (calKernal(outer.radius, perpixel) - i) / outer_area;
    // float y = singleGrowth(i, inner.growth_u, inner.growth_k) * dt + current.g;
    // float x = singleGrowth(k, outer.growth_u, outer.growth_k) * dt + current.b;

    float x = (2.0 * sigmoid_mix(
                        sigmoid_ab(k, outer.growth_u, outer.growth_k, 0.028, 0.028),
                        sigmoid_ab(k, inner.growth_u, inner.growth_k, 0.028, 0.028),
                        i, 0.147
                        ) - 1.0) * dt + current.b;

    // return clamp(vec3(k, i, x), 0, 1);
    return clamp(vec3(i * 0.8, x, x), 0, 1);
    // return clamp(vec3(k * i, y, x), 0, 1);
}

void main() {
    frag_color = vec4(smoothLife(), 1);
}
