#include "FixPoint.h"
#include "Circle.h"
#include "mfwlog.h"
#include "PhysicsEmulator.h"

void FixPoint::solve(const f64& dt) {
    TOVOID(dt);
    if (!target)
        return;
    target->m_pos = self.m_pos;
    target->m_acceleration = {};
    target->m_velocity = {};
}

void FixPoint::draw(const glm::mat4& proj, mfw::Renderer& renderer) {
    Circle(self.m_pos, self.m_color, PhysicsEmulator::sim->attri.node_size * 1.6).draw(proj, renderer);
}

