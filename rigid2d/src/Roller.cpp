#include "Roller.h"
#include "Circle.h"
#include "mfwlog.h"
#include "Simulation.h"

void Roller::solve(const f64& dt) {
    TOVOID(dt);
    if (!target)
        return;
    self.m_pos.x = target->m_pos.x;
    target->m_pos.y = self.m_pos.y;
    target->m_acceleration.y = 0;
    target->m_velocity.y = 0;
}

void Roller::draw(const glm::mat4& proj, mfw::Renderer& renderer) {
    Circle(self.m_pos, self.m_color, Simulation::Get()->attri.node_size * 1.6).draw(proj, renderer);
}

