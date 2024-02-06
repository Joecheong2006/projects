#version 330 core

uniform vec2 resolution;
uniform vec2 offset;
uniform float time;
uniform float zoom;

#define PI 3.1415f

layout(location = 0) out vec4 frag_color;

float line(vec2 p, vec2 p1, vec2 p2) {
    float m = (p1.y - p2.y) / (p1.x - p2.x);
    return m * (p.x - p1.x) - p.y + p1.y;
}

float distance_to_line(vec2 p, vec2 p1, vec2 p2) {
    float a = (p1.y - p2.y) / (p1.x - p2.x);
    float c = p2.y - p2.x * (p1.y - p2.y) / (p1.x - p2.x);
    if (p1.x == p2.x) {
        return abs(p1.x - p.x);
    }
    return abs((a * p.x - p.y + c)) / sqrt(a * a + 1);
}

float get_iterate(vec2 c) {
    float maxI = 100;
    vec2 z = vec2(0);
    for (float i = 0; i < maxI; ++i) {
        z = vec2(z.x * z.x - z.y * z.y, 2 * z.x * z.y) + c;
        if (z.x * z.x + z.y * z.y > 4) {
            return i / maxI;
        }
    }
    return maxI;
}


void main() {
    vec2 uv = (gl_FragCoord.xy / resolution - 0.5) * 4 * vec2(1, resolution.y / resolution.x);

    float value = smoothstep(0, 0.4, get_iterate(uv / zoom + offset));
    frag_color = vec4(vec3(value), 0);
}
