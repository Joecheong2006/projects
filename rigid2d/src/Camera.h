#pragma once
#include "glm/glm.hpp"

struct Camera {
    glm::mat4 ortho = glm::mat4(1);
    glm::mat4 view = glm::mat4(1);
    glm::mat4 scale = glm::mat4(1);
    inline const glm::mat4 getProjection() const { return ortho * scale * view; }
};

