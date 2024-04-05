#include "Object.h"

void Object::update([[maybe_unused]] const f64& dt)
{}

void Object::draw([[maybe_unused]] const glm::mat4& proj,
                  [[maybe_unused]] mfw::Renderer& renderer)
{}

i32 Object::getTypeId() const {
    return -1;
}
