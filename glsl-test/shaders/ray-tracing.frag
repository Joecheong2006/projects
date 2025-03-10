#version 330 core

#define PI 3.1415926

layout(location = 0) out vec4 frag_color;

uniform vec2 resolution;
uniform float time;

struct circle {
    vec3 center;
    float radius;
};

#define CIRCLE_LEN 2
circle circles[CIRCLE_LEN];

uint pcg(uint v) {
    uint state = v * uint(747796405) + uint(2891336453);
    uint word = ((state >> ((state >> uint(28)) + uint(4))) ^ state) * uint(277803737);
    return (word >> uint(22)) ^ word;
}

float rand(float p) {
    return float(pcg(uint(p + time))) / float(uint(0xffffffff));
}

float randND(float state) {
    float theta = 2 * PI * rand(state);
    float rho = sqrt(-2 * log(rand(state)));
    return rho * cos(theta);
}

vec2 rand2(vec2 p) {
    return vec2(randND(p.x), randND(p.y)); 
}

vec3 rand3(vec3 p) {
    return vec3(randND(p.x), randND(p.y), randND(p.z)); 
}

struct ray {
    vec3 origin, direction;
};

ray Ray(vec3 origin, vec3 direction) {
    ray result;
    result.origin = origin;
    result.direction = normalize(direction);
    return result;
}

vec3 ray_at(ray r, float t) {
    return r.origin + t * r.direction;
}

float hit_sphere(vec3 center, float radius, ray r) {
    vec3 dir = center - r.origin;
    float a = dot(r.direction, r.direction);
    float b = -2.0 * dot(r.direction, dir);
    float c = dot(dir, dir) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return -1;
    }
    return (-b - sqrt(discriminant)) / (2.0 * a);
}

vec3 ray_color(ray r) {
    float closest = 0xffffff;
    vec3 color = vec3(0.0);

    for (int b = 0; b < 2; ++b) {
        ray reflect_r = r;
        vec3 collect_color = vec3(1.0);
        for (int i = 0; i < CIRCLE_LEN; ++i) {
            float t = hit_sphere(circles[i].center, circles[i].radius, r);
            if (t >= 0 && closest > t) {
                vec3 normal = normalize(ray_at(r, t) - circles[i].center);
                collect_color = dot(-normalize(vec3(1, 1, 1)), normal) * collect_color;
                closest = t;

                vec3 nr = ray_at(r, t);
                vec3 n = dot(normal, -nr) * normal - nr;
                reflect_r = Ray(nr + 2.0 * n, nr);
            }
        }
        if (reflect_r == r) {
            break;
        }
        r = reflect_r;
        closest = 0xffffff;
        color += collect_color;
    }

    return color;
}

void main() {
    circles[1].center = vec3(-0.49, 0, 3);
    circles[1].radius = 0.5;

    circles[0].center = vec3(0.51, 0, 3);
    // circles[0].center = vec3(0.5, 0, 3) + vec3(0, 0, sin(time)) * 3;
    circles[0].radius = 0.5;

    // vec3 forward = vec3(cos(time), 0.0, sin(time));
    // vec3 camera_center = vec3(cos(time), 0, sin(time)) * 6;
    vec3 forward = vec3(0.0, 0.0, 1.0);
    vec3 camera_center = vec3(0.0, 0.0, 0);

    float viewport_ratio = resolution.x / resolution.y;
    float focal_length = length(forward - camera_center);
    float fov = 45.0;

    float viewport_height = 2.0 * tan((fov * PI / 180.0) / 2.0) * focal_length;
    float viewport_width = viewport_height * viewport_ratio;
    vec2 viewport = vec2(viewport_width, viewport_height);

    vec3 k = normalize(forward - camera_center);
    vec3 i = normalize(cross(k, vec3(0.0, 1.0, 0.0)));
    vec3 j = cross(k, i);

    vec3 uv = vec3(gl_FragCoord.xy / resolution * 2.0 - 1.0, 0);
    uv = viewport_width * 0.5 * i * uv.x + viewport_height * 0.5 * j * uv.y + focal_length * k;
    uv += camera_center;

    vec2 per_pixel = viewport / vec2(resolution.x, resolution.y);

    ray r = Ray(camera_center, uv + vec3(per_pixel, 0) * 0.5 - camera_center);
    vec3 color = ray_color(r);
    int pixelSimple = 0;
    for (int i = 0; i < pixelSimple; ++i) {
        // vec3 offset = vec3(rand2(gl_FragCoord.xy + vec2(i + time * 1000)) / resolution.xy + per_pixel * 0.5, 0);
        vec3 offset = vec3(rand2(gl_FragCoord.xy + vec2(i + time * 1000)) / resolution.xy, 0)
                    + per_pixel.x * i * 0.5 + per_pixel.y * j * 0.5;
        color += ray_color(Ray(
            camera_center,
            r.direction + offset - camera_center
        ));
    }

    frag_color = vec4(color / (pixelSimple + 1.0f), 1);
}

