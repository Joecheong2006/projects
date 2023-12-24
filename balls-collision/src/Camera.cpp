#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
    : m_view(glm::mat4(1)), m_projection(glm::mat4(1)), position(0, 0, 3), rotation(0, 0, -90), front(0, 0, -1), up(0, 1, 0)
{
}

Camera::Camera(glm::vec3 position)
    : Camera()
{
    this->position = position;
}

Camera::Camera(float x, float y, float z)
    : Camera()
{
    this->position = {x, y, z};
}

void Camera::update()
{
    m_view = glm::lookAt(position, position + front, up);
    m_projection = glm::perspective(glm::radians(fov), aspect, near, far);
}

void Camera::set_perspective(float fov, float aspect, float near, float far)
{
    this->fov = fov;
    this->aspect = aspect;
    this->near = near;
    this->far = far;
}

void Camera::rotate(glm::vec3 angle)
{
    rotation += angle;

    glm::vec3 direction;
    direction.x = cos(glm::radians(rotation.z)) * cos(glm::radians(rotation.y));
    direction.y = sin(glm::radians(rotation.y));
    direction.z = sin(glm::radians(rotation.z)) * cos(glm::radians(rotation.y));

    front = glm::normalize(direction);
}

