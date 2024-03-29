#include "Application.h"

#include "Renderer.h"
#include "Object.h"
#include "PointConstraint.h"
#include "Simulation.h"

class PhysicsEmulator : public mfw::Application {
private:
    struct Settings {
        i32 sub_step = 500;
        bool pause = false,
             gravity = true,
             world_view = true,
             velocity_view = false,
             acceleration_view = false;
    } settings;

    enum Mode {
        Normal,
        Action,
        Edit,
        None
    } mode;

    f64 sub_dt;
    glm::vec2 catch_offset;

    Object* holding = nullptr;
    std::vector<Object*> preview;

    mfw::Renderer renderer;

    // world
    f32 world_scale;
    f32 shift_rate;
    f32 zoom_rate;
    f32 zoom;

    void UpdateStatus();

    i32 width, height;
    glm::vec2 mouse;
    const f64 refresh_rate = 144, fps = refresh_rate;
    f64 frame = 1.0 / fps, render_frame = 0, update_frame = 0;

    void SetWorldProjection(glm::vec2 view);
    void ApplyUserInputToScene();
    void update(const f64& dt);
    void render();
    void renderImgui();

    void OnEdit(const mfw::MouseButtonEvent& event, const glm::dvec2& wpos);
    void OnNormal(const mfw::MouseButtonEvent& event, const glm::vec2& wpos);

public:
    PhysicsEmulator(Simulation* simluation);
    ~PhysicsEmulator();
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnInputKey(const mfw::KeyEvent& event) override;
    virtual void OnCursorMove(const mfw::CursorMoveEvent& event) override;
    virtual void OnMouseScroll(const mfw::MouseScrollEvent& event) override;
    virtual void OnMouseButton(const mfw::MouseButtonEvent& event) override;
    virtual void OnWindowResize(const mfw::WindowResizeEvent& event) override;
    virtual void OnWindowFocus(const mfw::WindowFocusEvent& event) override;
    virtual void OnWindowNotFocus(const mfw::WindowNotFocusEvent& event) override;

    static Simulation* simu;

};

