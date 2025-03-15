#version 330 core

#define PI 3.1415926
#define RAD(x) ((x) * PI / 180.0)

layout(location = 0) out vec4 frag_color;

uniform vec2 resolution;
uniform float time;

struct hit_info {
    vec3 point, normal;
    float t;
};

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

float rand(inout float p) {
    return float(pcg(uint(p))) / float(uint(0xffffffff));
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

bool hit_sphere_circle(in circle cir, ray r, float max, out hit_info info) {
    vec3 dir = cir.center - r.origin;
    float a = dot(r.direction, r.direction);
    float b = -2.0 * dot(r.direction, dir);
    float c = dot(dir, dir) - cir.radius * cir.radius;
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return false;
    }

    float sqrtd = sqrt(discriminant);
    info.t = (-b - sqrtd) / (2.0 * a);

    if (!(info.t > 0 && info.t < max)) {
        info.t = (-b + sqrtd) / (2.0 * a);
        if (!(info.t > 0 && info.t < max)) {
            return false;
        }
    }

    info.point = ray_at(r, info.t);
    info.normal = (info.point - cir.center) / cir.radius;
    if (dot(r.direction, info.normal) > 0) {
        info.normal = -info.normal;
    }
    return true;
}

bool hit(ray r, out hit_info track) {
    hit_info tmp;

    float closest = 0xffffff;
    bool hit_something = false;

    for (int i = 0; i < CIRCLE_LEN; ++i) {
        if (hit_sphere_circle(circles[i], r, closest, tmp)) {
            hit_something = true;
            closest = tmp.t;
            track = tmp;
        }
    }

    return hit_something;
}

vec3 trace_color(ray r) {
    vec3 color = vec3(0.0);

    float m = 1.0;

    for (int i = 0; i < 20; ++i) {
        hit_info info;
        if (hit(r, info)) {
            color += dot(-normalize(vec3(1, 1, 1)), info.normal) * vec3(1.0) * m;

            m *= 0.7;

            r.origin = info.point + info.normal * 0.001;
            r.direction -= 2.0 * dot(info.normal, r.direction) * info.normal;
            normalize(r.direction);
        }
        else {
            break;
        }
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
    vec3 camera_center = vec3(sin(time * 1.2), 0, -cos(time * 1.2)) * 6;
    vec3 forward = vec3(0.0, 0.0, 1.0);
    // vec3 camera_center = vec3(0.0, 0.0, 0);

    float viewport_ratio = resolution.x / resolution.y;
    float focal_length = length(forward - camera_center);
    float fov = 45.0;

    float viewport_height = 2.0 * tan(RAD(fov) / 2.0) * focal_length;
    float viewport_width = viewport_height * viewport_ratio;
    vec2 viewport = vec2(viewport_width, viewport_height);

    vec3 k = normalize(forward - camera_center);
    vec3 i = normalize(cross(k, vec3(0.0, 1.0, 0.0)));
    vec3 j = cross(k, i);

    vec3 uv = vec3(gl_FragCoord.xy / resolution * 2.0 - 1.0, 0);
    uv = viewport_width * 0.5 * i * uv.x + viewport_height * 0.5 * j * uv.y + focal_length * k;
    uv += camera_center;

    vec2 per_pixel = viewport / vec2(resolution.x, resolution.y);
    ray r = Ray(camera_center, uv + (per_pixel.x * i + per_pixel.y * j) * 0.5 - camera_center);

    vec3 color = trace_color(r);
    int pixelSimple = 0;

    for (int i = 0; i < pixelSimple; ++i) {
        color += trace_color(Ray(
            camera_center,
            uv + ((per_pixel.x + randND(gl_FragCoord.x + i + time * 1000) / resolution.x) * i
               + (per_pixel.y + randND(gl_FragCoord.y + i + time * 1000) / resolution.y) * j) * 0.5
               - camera_center
        ));
    }

    frag_color = vec4(color / (pixelSimple + 1.0f), 1);
}

