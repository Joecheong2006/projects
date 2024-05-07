#pragma once
#include "util.h"

struct Camera {
    mat4 ortho = mat4(1);
    mat4 view = mat4(1);
    mat4 scale = mat4(1);
    inline const mat4 getProjection() const { return ortho * scale * view; }
};

