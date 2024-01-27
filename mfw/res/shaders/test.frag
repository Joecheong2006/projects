#version 330 core

uniform vec2 resolution;
uniform float time;

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

void main() {
    vec2 uv = (gl_FragCoord.xy / resolution - 0.5) * 2.0 * vec2(1, resolution.y / resolution.x);

    vec3 color = vec3(0.0);

    color += 1 - smoothstep(0, 0.01, distance_to_line(uv, vec2(0), vec2(cos(time), sin(time))));

    frag_color = vec4(color, 1.0);
}
