#include "PhysicsEmulator.h"
#include "mp/TestCollision2D.h"

#include "DoublePendulum.h"
#include "Cradle.h"
#include "Cloth.h"
#include "PoolBalls.h"

template <>
struct Log::Pattern<vec2> {
    static void Log(const vec2& value, const std::string& format) {
        (void)format;
        LOG_INFO("[{}, {}]", value.x, value.y);
    }
};

class Demo : public Simulation {
public:
    Demo(): Simulation(__func__, 0.2) {}
    virtual void OnStart() override {
        SetDefaultConfig();
        world.setSubStep(10);
        world.setObjectLayer<DistanceConstraint>(RenderLayer::Level2);
        world.setObjectLayer<Spring>(RenderLayer::Level1);
        world.setObjectLayer<Circle>(RenderLayer::Level3);
        world.setObjectLayer<Cylinder>(RenderLayer::Level8);

        world.addRigidBody<Circle>(vec2())->setStatic();
        world.addRigidBody<Circle>(vec2(), 0.1)->setStatic();
        world.addRigidBody<Cylinder>(vec2(0, -1), vec2(1, 0))->setStatic();
    }

    virtual void OnRender(mfw::Renderer& renderer) override {
        auto circle1 = world.findObjects<Circle>()[0];
        auto circle2 = world.findObjects<Circle>()[1];
        auto cylinder = world.findObjects<Cylinder>()[0];

        auto aabb1 = circle1->collider.getBounds();
        auto aabb2 = circle2->collider.getBounds();

        auto& proj = camera.getProjection();

        {
            auto aabb3 = cylinder->collider.getBounds();
            i32 is_collide = TestCollision2D::AABB(aabb2, aabb3);
            renderer.renderLine(proj, aabb3.min, aabb3.max, color(1 * is_collide, 1, 0), 0.005);
            aabb3.min.y += aabb3.size.y;
            aabb3.max.y -= aabb3.size.y;
            renderer.renderLine(proj, aabb3.min, aabb3.max, color(1 * is_collide, 1, 0), 0.005);
        }

        auto is_collide = TestCollision2D::AABB(aabb2, aabb1);

        renderer.renderLine(proj, aabb1.min, aabb1.max, color(1 * is_collide, 1, 0), 0.005);
        aabb1.min.y += aabb1.size.y;
        aabb1.max.y -= aabb1.size.y;
        renderer.renderLine(proj, aabb1.min, aabb1.max, color(1 * is_collide, 1, 0), 0.005);

        renderer.renderLine(proj, aabb2.min, aabb2.max, color(1 * is_collide, 1, 0), 0.005);
        aabb2.min.y += aabb2.size.y;
        aabb2.max.y -= aabb2.size.y;
        renderer.renderLine(proj, aabb2.min, aabb2.max, color(1 * is_collide, 1, 0), 0.005);
    }

};

mfw::Application* mfw::CreateApplication() {
    PhysicsEmulator* emulator = new PhysicsEmulator();
    // emulator->setSimulation(new PoolBalls);
    emulator->setSimulation(new Cradle);
    emulator->world_scale = 13;
    emulator->shift_rate = 0.01;
    emulator->zoom_rate = 0.03;

    return emulator;
}

