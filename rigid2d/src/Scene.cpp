#include "Scene.h"

#include "DistanceConstraint.h"
#include "PointConstraint.h"
#include "Renderer.h"
#include "Circle.h"
#include "mfwlog.h"
#include "Input.h"
#include "Application.h"

void Scene::InitString(World& world, const glm::vec2& pos, u32 node, f32 length) {
    ASSERT(node > 0);

    auto& circles = world.getObjects<Circle>();
    auto& dcs = world.getConstraint<DistanceConstraint>();
    u32 first = circles.size();

    for (f32 i = 0; i < node; i++) {
        circles.push_back(new Circle(glm::vec2(0, -i * length) + pos, attri.node_color, attri.node_size));
    }
    for (u32 i = 0; i < node - 1; i++) {
        dcs.emplace_back(new DistanceConstraint(circles[first + i], circles[first + i + 1], length));
    }
}

void Scene::InitCircle(World& world, const glm::vec2& pos, i32 n, f32 r, i32 nstep) {
    ASSERT(r > 0 && n > 2 && nstep > 0);
    static const f32 pi = 3.14159265359f;

    auto& circles = world.getObjects<Circle>();
    auto& dcs = world.getConstraint<DistanceConstraint>();
    u32 first = circles.size();

    f32 ri = 2 * pi / n;

    for (i32 i = 0; i < n; i++) {
        f32 a = ri * i;
        circles.push_back(new Circle(r * glm::vec2(sin(a), cos(a)) + pos, attri.node_color, attri.node_size));
    }
    circles.push_back(new Circle(glm::vec2(0) + pos, attri.node_color, attri.node_size));

    for (i32 i = 0; i < n; i++) {
        dcs.push_back(new DistanceConstraint(circles[first + i], circles[first + n], r));
    }

    for (i32 step = 1; step <= nstep; step++) {
        f32 nlen = glm::length(r * glm::vec2(sin(0) - sin(ri * step), cos(0) - cos(ri * step)));
        for (i32 i = 0; i < n; i++) {
            dcs.push_back(new DistanceConstraint(circles[first + i], circles[first + (i + step) % n], nlen));
        }
    }
}

void Scene::InitBox(World& mesh, const glm::vec2& pos, f32 l)
{
    ASSERT(l > 0);
    InitCircle(mesh, pos, 4, sqrt(2.0f) * l * 0.5);
}

void Scene::InitTriangle(World& mesh, const glm::vec2& pos, f32 l) {
    ASSERT(l > 0);
    InitCircle(mesh, pos, 3, l);
}

PointConstraint* Scene::AddHorizontalPointConstraint(World& world, const glm::dvec2& pos, f32 r) {
    auto result = world.addConstraint<PointConstraint>(r, [](const f64& dt, PointConstraint* pc) {
                    (void) dt;
                    if (pc->target) {
                        pc->self.m_pos.x = pc->target->m_pos.x;
                        pc->target->m_pos.y = pc->self.m_pos.y;
                        pc->target->m_acceleration.y = 0;
                        pc->target->m_velocity.y = 0;
                    }
                });
    result->self = Object(pos, 0, glm::vec3(COLOR(0x3c4467)));
    return result;
}

PointConstraint* Scene::AddFixPointConstraint(World& world, const glm::dvec2& pos, f32 r) {
    auto result = world.addConstraint<PointConstraint>(r, [](const f64& dt, PointConstraint* pc) {
                    (void) dt;
                    if (pc->target) {
                        pc->target->m_pos = pc->self.m_pos;
                        pc->target->m_acceleration = {};
                        pc->target->m_velocity = {};
                    }
                });
    result->self = Object(pos, 0, glm::vec3(COLOR(0x486577)));
    return result;
}

glm::dvec2 Scene::mouseToWorldCoord(mfw::Window* main) {
    auto& mouse = mfw::Input::GetMouse();
    f32 width = main->width(), height = main->height();
    glm::vec4 uv = glm::vec4(mouse.first / width, 1 - mouse.second / height, 0, 0) * 2.0f - 1.0f;
    return camera.view * (uv * ((glm::vec4(1) / camera.ortho) / camera.scale));
}

