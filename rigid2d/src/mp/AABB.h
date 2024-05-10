#pragma once
#include "util.h"

struct AABB {
    AABB() = default;
    AABB(vec2 size, vec2 center)
        : size(size), center(center), extends(size * 0.5), max(center + extends), min(center - extends)
    {}
    vec2 size, center, extends, max, min;

};

