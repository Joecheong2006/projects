#version 330 core

uniform vec2 resolution;
uniform float time;

#define PI 3.1415f

layout(location = 0) out vec4 frag_color;

void main() {
    vec2 uv = (gl_FragCoord.xy / resolution - 0.5) * 2.0 * vec2(1, resolution.y / resolution.x);

    vec3 color = vec3(0.0);

    color += 1 - smoothstep(0, 0.005, length(uv) - 0.4);

    frag_color = vec4(color, 1.0);
}
