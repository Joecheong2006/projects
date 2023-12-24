#pragma once

#include <glm/glm.hpp>

class Camera
{
private:
    glm::mat4 m_view, m_projection;

public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 front;
    glm::vec3 up;

    float fov, aspect, near, far;

    Camera();
    Camera(glm::vec3 position);
    Camera(float x, float y, float z);

    void update();
    void set_perspective(float fov, float aspect, float near, float far);
    void rotate(glm::vec3 angle);

    const glm::mat4& view() const { return m_view; }
    const glm::mat4& projection() const { return m_projection; }

};
