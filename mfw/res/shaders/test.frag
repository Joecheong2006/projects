#version 330 core

uniform vec2 resolution;
uniform vec2 offset;
uniform float time;
uniform float zoom;
in vec2 texture_coord;

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
    vec3 color = vec3(1);
    float r = 1 - smoothstep(0.85, 0.9, length(texture_coord * 2 - 1));
    frag_color = vec4(vec3(r * color), r);
    return;
    vec2 uv = offset + (gl_FragCoord.xy / resolution - 0.5) * 4 * vec2(1, resolution.y / resolution.x);
    uv = uv * zoom * 4;

    float x = uv.x, y = uv.y;
    float e = sin(y + time) * cos(x + time) - cos(time);

    float value = 0;
    value += 1 - smoothstep(0, 0.03, abs(e));

    value = max((1 - min(step(0.01, distance_to_line(uv, vec2(0), vec2(0, 1))),
                     step(0.01, distance_to_line(uv, vec2(0), vec2(1, 0))))) * 0.3, value);

    //float value = smoothstep(0, 0.4, get_iterate(uv / pow(zoom, zoom) + offset));
    frag_color = vec4(vec3(value), 1);
}
