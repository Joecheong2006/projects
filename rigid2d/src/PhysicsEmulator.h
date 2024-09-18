#include <mfw/Application.h>

#include "mp/Renderer.h"
#include "mp/RigidBody2D.h"
#include "mp/PointConstraint.h"
#include "Simulation.h"

class PhysicsEmulator : public mfw::Application {
public:
    PhysicsEmulator();
    ~PhysicsEmulator();
    void setSimulation(Simulation* simulation);
    virtual void Start() override;
    virtual void Update() override;
    virtual bool OnInputKey(const mfw::KeyEvent& event) override;
    virtual bool OnCursorMove(const mfw::CursorMoveEvent& event) override;
    virtual bool OnMouseScroll(const mfw::MouseScrollEvent& event) override;
    virtual bool OnMouseButton(const mfw::MouseButtonEvent& event) override;
    virtual bool OnWindowResize(const mfw::WindowResizeEvent& event) override;
    virtual bool OnWindowFocus(const mfw::WindowFocusEvent& event) override;
    virtual bool OnWindowNotFocus(const mfw::WindowNotFocusEvent& event) override;

    real world_scale = 0;
    real shift_rate;
    real zoom_rate;

    struct Settings {
        bool pause = false,
             gravity = true,
             world_view = true,
             velocity_view = false,
             acceleration_view = false;
        real mouseSpringStiffness = 3,
             mouseSpringDamping = 0.01;
    } settings;

private:
    enum Mode {
        Normal,
        Action,
        Edit,
        None
    } mode;

    real sub_dt;
    vec2 catch_offset;

    RigidBody2D* rigidBodyHolder = nullptr;
    PointConstraint* pointHolder = nullptr;
    std::vector<RigidBody2D*> preview;
    Simulation* simulation;

    mfw::Renderer renderer;

    void UpdateStatus();

    // window status
    i32 width, height;
    vec2 mouse;
    const real refresh_rate = 165, fps = refresh_rate;
    real frame = 1.0 / fps, render_frame = 0, update_frame = 0;

    void SetWorldProjection(vec2 view);
    void ApplySpringForce();
    void MovePointConstraint();
    void update(const real& dt);
    void render();
    void renderImgui();
    void restart();

    void OnEdit(const mfw::MouseButtonEvent& event, const vec2& wpos);
    void OnNormal(const mfw::MouseButtonEvent& event, const vec2& wpos);

    class Info : public mfw::UiLayer {
    public:
        Info(): UiLayer(__func__)
        {}
        virtual bool OnUiRender() override;

    };

};

