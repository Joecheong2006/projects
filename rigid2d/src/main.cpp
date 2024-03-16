#include <mfw.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "World.h"
#include "Renderer.h"

#include "Circle.h"
#include "DistanceConstraint.h"
#include "PointConstraint.h"

const glm::vec3 red = glm::vec3(COLOR(0xff0000));
const glm::vec3 green = glm::vec3(COLOR(0x00ff00));
const glm::vec3 blue = glm::vec3(COLOR(0x0000ff));

using namespace mfw;
class SandBox : public Application {
private:
    void update_status() {
        Window* main = GetWindow();
        width = main->width();
        height = main->height();
        auto& mouse = Input::GetMouse();
        this->mouse.x = mouse.first;
        this->mouse.y = mouse.second;
    }

public:
    i32 width, height;
    glm::vec2 mouse;
    const f64 refresh_rate = 144, fps = refresh_rate;
    f64 frame = 1.0 / fps, render_frame = 0, update_frame = 0;

    SandBox(): Application("rigid2d", 1440, 960)
    {}

    virtual void Start() override {
        update_status();
        //Get()->GetWindow()->setVSync(true);
    };

    virtual void Update() override {
        static f32 start = Time::GetCurrent();
        update_status();
        update(1.0 / fps); 
        render();
        renderImgui();
        frame = Time::GetCurrent() - start;
        start = Time::GetCurrent();
        Time::Sleep(1);
    }

    virtual void update(const f64& dt) = 0;
    virtual void render() = 0;
    virtual void renderImgui() = 0;

    ~SandBox() {
    }

};

class Rigid2DSimlution : public SandBox {
private:
    struct Settings {
        i32 sub_step = 100;
        f32 time_rate = 1.0;
        bool pause = false,
             gravity = true,
             world_view = true,
             velocity_view = false,
             acceleration_view = false;
    } settings;

    struct Attribute {
        glm::vec4 node_color;
        f32 node_size;
        f32 hardness;
        f32 line_width;
    };

    enum Mode {
        Normal,
        Action,
        Edit,
        None
    };

    // camera
    glm::mat4 o;
    glm::mat4 view;
    glm::mat4 scale;

    Mode mode;
    f64 sub_dt;
    glm::vec2 catch_offset;
    f32 unitScale = 0.1f;

    mfw::Renderer renderer;

    // world
    f32 world_scale = 16 * unitScale;
    f32 shift_rate = 0.001 * world_scale;
    f32 zoom_rate = 0.03 * world_scale;
    World world;

    Object* holding = nullptr;
    std::vector<Object*> preview;
    Attribute attri;

    Circle* preview_node = nullptr;
    PointConstraint* preview_fix_point = nullptr;
    glm::dvec2 preview_pos;

public:
    Rigid2DSimlution()
        : o(glm::mat4(1)), view(o), scale(o), mode(Mode::Normal), world(glm::vec2(30, 14) * unitScale)
    {}

    virtual void Start() override {
        SandBox::Start();

        SetWorldProjection(glm::vec2(width, height));
        preview.reserve(16);
        SetDefaultStickAttribute();
        InitializeObjects();

        glClearColor(0.1, 0.1, 0.1, 1);
    }

    void SetupDoublePendulum() {
        f64 r = -3.14 / 2, d = 4;
        glm::dvec2 direction = glm::normalize(glm::dvec2(cos(r), sin(r))) * d * (f64)unitScale;

        auto p1 = world.addObject(new Circle(glm::vec2(), attri.node_color, attri.node_size));
        auto p2 = world.addObject(new Circle(direction, attri.node_color, attri.node_size));
        auto p3 = world.addObject(new Circle(direction * 2.0, attri.node_color, attri.node_size));
        world.addConstraint(new DistanceConstraint(p1, p2, d * unitScale));
        world.addConstraint(new DistanceConstraint(p2, p3, d * unitScale));
        AddFixPointConstraint(world, glm::vec2(), 0.5 * unitScale)
            ->target = p1;
    }

    void SetDefaultStickAttribute() {
        attri.node_color = glm::vec4(glm::vec4(COLOR(0x858AA6), 0));
        attri.node_size = 0.34 * unitScale;
        attri.line_width = 0.12 * unitScale;
        attri.hardness = 1;
    }

    void InitializeObjects() {
        InitBox(world, glm::vec2(), 5 * unitScale);
        auto& boxCenter = world.getObjects<Circle>().back();
        auto& c1 = world.getObjects<Circle>()[0];
        auto& c2 = world.getObjects<Circle>()[2];
        AddFixPointConstraint(world, glm::vec2(), 0.5 * unitScale)
            ->target = boxCenter;

        auto h1 = AddHorizontalPointConstraint(world, glm::vec2(9, 0) * unitScale, 0.5 * unitScale);
        auto h2 = AddHorizontalPointConstraint(world, glm::vec2(-9, 0) * unitScale, 0.5 * unitScale);
        auto p1 = world.addObject(new Circle(h1->self.m_pos, attri.node_color, attri.node_size));
        auto p2 = world.addObject(new Circle(h2->self.m_pos, attri.node_color, attri.node_size));
        f64 l = glm::length(p1->m_pos - c1->m_pos);
        world.addConstraint(new DistanceConstraint(c1, p1, l));
        world.addConstraint(new DistanceConstraint(c2, p2, l));
        h1->target = p1;
        h2->target = p2;

        return;
        InitTriangle(world, glm::vec2(0), 3 * unitScale);
        InitTriangle(world, glm::vec2(0), 3 * unitScale);

        InitTriangle(world, glm::vec2(0), 3 * unitScale);
        InitBox(world, glm::vec2(-4, 0) * unitScale, 4 * unitScale);
        InitCircle(world, glm::vec2(), 6, 3 * unitScale);
        //InitializePointConstraint();
    }

    void InitializePointConstraint() {
        world.addConstraint(new PointConstraint(0.5 * unitScale, [=](const f64& dt, Object* self, Object* target) {
                        (void)dt; (void)self; (void)target;
                    }));
        world.addConstraint(new PointConstraint(0.5 * unitScale, [=](const f64& dt, Object* self, Object* target) {
                        (void)dt; (void)self; (void)target;
                    }));
    }

    void SetWorldProjection(glm::vec2 view) {
        glm::vec2 world = world_scale * glm::vec2(view.x, view.y) / view.y;
        o = glm::ortho(-world.x, world.x, -world.y, world.y, -1.0f, 1.0f);
    }

    glm::dvec2 MouseToWorldCoord(glm::vec2 mouse) {
        glm::vec4 uv = glm::vec4(mouse.x / width, 1 - mouse.y / height, 0, 0) * 2.0f - 1.0f;
        return view * ((uv / o) / scale);
    }

    void wall_collision(f64 dt, Circle* c, const glm::vec2& world) {
        if (c->m_pos.y - c->r < -world.y) {
            f64 ay = ((-world.y - c->m_pos.y + c->r) - (c->m_pos.y - c->m_opos.y)) / (dt * dt);
            c->addForce(glm::dvec2(0, ay * c->m_mass));
        }
        //else if (c->m_pos.y + c->r > world.y) {
        //    f64 ay = ((world.y - c->m_pos.y - c->r) - (c->m_pos.y - c->m_opos.y)) / (dt * dt);
        //    c->addForce(glm::dvec2(0, ay * c->m_mass));
        //}
        if (c->m_pos.x - c->r < -world.x) {
            f64 ax = ((-world.x - c->m_pos.x + c->r) - (c->m_pos.x - c->m_opos.x)) / (dt * dt);
            c->addForce(glm::dvec2(ax * c->m_mass, 0));
        }
        else if (c->m_pos.x + c->r > world.x) {
            f64 ax = ((world.x - c->m_pos.x - c->r) - (c->m_pos.x - c->m_opos.x)) / (dt * dt);
            c->addForce(glm::dvec2(ax * c->m_mass, 0));
        }
    };

    virtual void update(const f64& dt) override {
        if (settings.pause)
            return;
        Timer timer;
        if (mode != Mode::Edit) {
            sub_dt = dt * (f64)settings.time_rate / settings.sub_step;
            for (f64 i = 0; i <= frame; i += sub_dt) {
                world.update(sub_dt);
                auto& objects = world.getObjects<Circle>();
                for (auto& obj : objects) {
                    wall_collision(sub_dt, static_cast<Circle*>(obj), world.size);
                }
            }
            if (holding) {
                glm::dvec2 wpos = MouseToWorldCoord(mouse);
                if (holding->getType() == ObjectType::None) {
                    holding->m_pos = wpos - (glm::dvec2)catch_offset;
                }
                else {
                    f64 k = (f64)settings.sub_step * 200;
                    holding->addForce((wpos - (glm::dvec2)catch_offset - holding->m_pos) * k);
                }
            }
        }

        if (!Input::KeyPress(' ') && !Input::KeyPress(VK_CONTROL)) {
            mode = Mode::Normal;
        }
        update_frame = timer.getDuration();
    }

    virtual void render() override {
        Timer timer;
        renderer.clear();

        glm::mat4 proj = o * scale * view;

        if (settings.world_view) {
            for (auto& obj : world.getConstraint<PointConstraint>()) {
                const PointConstraint* point = static_cast<PointConstraint*>(obj);
                point->render(proj, renderer);
            }
            for (auto& obj : world.getConstraint<DistanceConstraint>()) {
                const DistanceConstraint* dc = static_cast<DistanceConstraint*>(obj);
                dc->render(proj, renderer, attri.line_width);
            }
            for (auto& obj : world.getObjects<Circle>()) {
                const Circle* circle = static_cast<Circle*>(obj);
                circle->render(proj, renderer);
            }
            renderer.renderRactangle(proj, glm::vec2(world.size.x, -world.size.y) - 0.2f * unitScale, glm::vec2(-world.size.x, -world.size.y) - 0.2f * unitScale, glm::vec4(1), 0.2 * unitScale);
        }

        if (settings.velocity_view || settings.acceleration_view) {
            auto& objects = world.getObjects<Circle>();
            for (auto& obj : objects) {
                const Circle* circle = static_cast<Circle*>(obj);
                if (settings.velocity_view) {
                    renderer.renderRactangle(proj, circle->m_pos, circle->m_pos + circle->m_velocity * (f64)unitScale, red, 0.05 * unitScale);
                }
                if (settings.acceleration_view) {
                    glm::dvec2 a = ((circle->m_pos - circle->m_opos) / sub_dt - circle->m_velocity) / sub_dt;
                    if (glm::length(a) > 1) {
                        a = glm::normalize(a);
                    }
                    renderer.renderRactangle(proj, circle->m_pos, circle->m_pos + a * (f64)unitScale, blue, 0.05 * unitScale);
                }
            }
        }

        render_frame = timer.getDuration();
    }

    std::vector<Circle*> FindCirclesByPosition(const glm::vec2& pos) {
        std::vector<Circle*> result;
        for (auto& obj : world.getObjects<Circle>()) {
            auto circle = static_cast<Circle*>(obj);
            if (circle->r > glm::length((const glm::vec2&)circle->m_pos - pos)) {
                result.push_back(circle);
            }
        }
        return result;
    }


    virtual void renderImgui() override {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("config");

        ImGui::Text("SF/%d ST[ms]/%.1g", (i32)(1.0 / sub_dt), 1000.0f * update_frame);
        ImGui::Text("FPS/%-5.1f", 1.0 / frame);
        ImGui::Text("sticks: %d", GetSticksCount());
        ImGui::Text("unit: %gcm", unitScale * 100);

        ImGui::Checkbox("pause", &settings.pause);
        ImGui::Checkbox("gravity", &settings.gravity);
        ImGui::Checkbox("world view", &settings.world_view);
        ImGui::Checkbox("velocity view", &settings.velocity_view);
        ImGui::Checkbox("acceleration view", &settings.acceleration_view);
        ImGui::SliderFloat("time rate", &settings.time_rate, 0.1, 1);
        ImGui::SliderInt("sub step", &settings.sub_step, 1, 1000);

        bool motified = false;
        motified |= ImGui::SliderFloat("hardness", &attri.hardness, 0.1f, 1.0f);
        motified |= ImGui::SliderFloat("line width", &attri.line_width, 0.01f * unitScale, 0.3f * unitScale);
        motified |= ImGui::SliderFloat("node size", &attri.node_size, 0.1f * unitScale, 1 * unitScale);
        motified |= ImGui::ColorEdit3("node color", glm::value_ptr(attri.node_color));

        if (motified) {
            preview.clear();
            for (auto& obj : world.getObjects<Circle>()) {
                Circle* circle = static_cast<Circle*>(obj);
                circle->m_color = attri.node_color;
                circle->r = attri.node_size;
            }
        }

        if (ImGui::Button("restart")) {
            world.clear();
            SetDefaultStickAttribute();
            InitializeObjects();
            preview.clear();
            mode = Mode::Normal;
        }

        if (ImGui::Button("reset")) {
            settings = Settings();
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (mode != Mode::Action) {
            auto objects = FindCirclesByPosition(MouseToWorldCoord(mouse));
            for (auto& prev : preview) {
                prev->m_color = attri.node_color;
            }
            for (auto& circle : objects) {
                preview.emplace_back(circle);
            }
            for (auto& circle : objects) {
                circle->m_color = glm::vec4(COLOR(0x5D627E), 0);
            }
        }
    }

    i32 GetSticksCount() {
        auto& objects = world.getConstraint<DistanceConstraint>();
        return objects.size();;
    }

    virtual void OnInputKey(const KeyEvent& event) override {
        if (event.key == VK_ESCAPE && event.mode == KeyMode::Down) {
            Terminate();
        }

        if (event.key == 'S' && event.mode == KeyMode::Down) {
            settings.pause = true;
        }
        else if (event.key == 'S' && event.mode == KeyMode::Release) {
            settings.pause = false;
        }
        
        if (mode == Mode::Normal) {
            if (event.key == VK_CONTROL && event.mode == Down && !holding) {
                mode = Mode::Edit;
            }
            if (event.key == ' ' && event.mode == Down && !holding) {
                for (auto& prev : preview) {
                    prev->m_color = attri.node_color;
                }
                mode = Mode::Action;
            }
            return;
        }

    }

    virtual void OnCursorMove(const CursorMoveEvent& event) override {
        static glm::vec2 m = glm::vec2(event.x, event.y);
        if (mode == Mode::Action && Input::MouseButtonDown(Left)) {
            view = glm::translate(view, glm::vec3(event.x - m.x, m.y - event.y, 0) * shift_rate);
        }
        m = glm::vec2(event.x, event.y);
    }

    virtual void OnMouseScroll(const MouseScrollEvent& event) override {
        if (mode == Mode::Action) {
            f32 val = 1 + event.ydelta * zoom_rate;
            scale = glm::scale(scale, glm::vec3(val, val, 1));
        }
    }

    PointConstraint* FindPointConstraintByPosition(const glm::vec2& pos) {
        auto& objects = world.getConstraint<PointConstraint>();
        for (auto& obj : objects) {
            auto point = static_cast<PointConstraint*>(obj);
            if (point->d > glm::length((glm::vec2)point->self.m_pos - pos)) {
                return point;
            }
        }
        return nullptr;
    }

    Circle* FindCircleByPosition(const glm::vec2& pos) {
        auto& objects = world.getObjects<Circle>();
        for (auto& obj : objects) {
            auto point = static_cast<Circle*>(obj);
            if (point->r > glm::length((const glm::vec2&)point->m_pos - pos)) {
                return point;
            }
        }
        return nullptr;
    }

    virtual void OnMouseButton(const MouseButtonEvent& event) override {
        glm::dvec2 wpos = MouseToWorldCoord(mouse);

        switch (mode) {
            case Mode::Normal:
                OnNormal(event, wpos);
                break;
            case Mode::Edit:
                OnEdit(event, wpos);
                break;
            default:
                break;
        }
    }

    void OnEdit(const MouseButtonEvent& event, const glm::dvec2& wpos) {
        if (event.button == MouseButton::Left && event.mode == KeyMode::Down) {
            preview_node = FindCircleByPosition(wpos);
            preview_fix_point = FindPointConstraintByPosition(wpos);
            if (!preview_node) {
                preview_pos = wpos;
            }
        } else if (event.button == MouseButton::Left && event.mode == KeyMode::Release) {
            Circle* second_node = FindCircleByPosition(wpos);
            PointConstraint* point = FindPointConstraintByPosition(wpos);

            if (preview_node && second_node) {
                if (preview_node == second_node) {
                    return;
                }
                f64 d = glm::length(preview_node->m_pos - second_node->m_pos);
                world.addConstraint(new DistanceConstraint(preview_node, second_node, d));
            }
            else if (preview_node && second_node == nullptr) {
                f64 d = glm::length(preview_node->m_pos - wpos);
                auto p = world.addObject(new Circle(wpos, attri.node_color, attri.node_size));
                auto dc = world.addConstraint(new DistanceConstraint(preview_node, p, d));

                p->m_color = attri.node_color;
                p->r = attri.node_size;

                if (point) {
                    point->target = p;
                    dc->d = glm::length(point->self.m_pos - preview_node->m_pos);
                }
            }
            else if (preview_node == nullptr && second_node) {
                f64 d = glm::length(preview_pos - second_node->m_pos);
                auto p = world.addObject(new Circle(preview_pos, attri.node_color, attri.node_size));
                auto dc = world.addConstraint(new DistanceConstraint(p, second_node, d));

                p->m_color = attri.node_color;
                p->r = attri.node_size;

                if (preview_fix_point) {
                    preview_fix_point->target = p;
                    dc->d = glm::length(preview_fix_point->self.m_pos - second_node->m_pos);
                }
            }
            else {
                if (preview_pos == wpos) {
                    world.addObject(new Circle(preview_pos, attri.node_color, attri.node_size));
                    return;
                }
                f32 d = glm::length(preview_pos - wpos);
                if (d < attri.node_size * 2) {
                    return;
                }

                auto p1 = world.addObject(new Circle(preview_pos, attri.node_color, attri.node_size));
                auto p2 = world.addObject(new Circle(wpos, attri.node_color, attri.node_size));
                auto dc = world.addConstraint(new DistanceConstraint(p1, p2, d));

                p1->m_color = attri.node_color;
                p1->r = attri.node_size;
                p2->m_color = attri.node_color;
                p2->r = attri.node_size;

                if (point && preview_fix_point) {
                    point->target = static_cast<Object*>(p2);
                    preview_fix_point->target = static_cast<Object*>(p2);
                    dc->d = glm::length(point->self.m_pos - preview_fix_point->self.m_pos);
                }
                else if (preview_fix_point) {
                    preview_fix_point->target = static_cast<Object*>(p2);
                    dc->d = glm::length(preview_fix_point->self.m_pos - wpos);
                }
                else if (point) {
                    point->target = static_cast<Object*>(p2);
                    dc->d = glm::length(point->self.m_pos - preview_node->m_pos);
                }
            }

        }
    }

    void OnNormal(const MouseButtonEvent& event, const glm::vec2& wpos) {
        if (event.button == MouseButton::Left && event.mode == KeyMode::Release) {
            if (holding) {
                PointConstraint* point = FindPointConstraintByPosition(holding->m_pos);
                if (point) {
                    point->target = holding;
                    holding = nullptr;
                }
            }
        }

        if (event.button == MouseButton::Left && event.mode == KeyMode::Down) {
            if (!holding) {
                PointConstraint* point = FindPointConstraintByPosition(wpos);
                if (point) {
                    holding = point->target;
                    point->target = nullptr;
                }
            }
        }

        if (event.button == MouseButton::Left && event.mode == KeyMode::Down) {
            if (!holding) {
                holding = FindCircleByPosition(wpos);
                if (holding) {
                    catch_offset = wpos - (glm::vec2)holding->m_pos;
                }
            }
        } else {
            holding = nullptr;
        }

        if (event.button == MouseButton::Right && event.mode == KeyMode::Down) {
            if (!holding) {
                auto pc = FindPointConstraintByPosition(wpos);
                if (pc) {
                    holding = &pc->self;
                }
            }
        }
    }

    virtual void OnWindowResize(const WindowResizeEvent& event) override {
        glViewport(0, 0, event.width, event.height);
        SetWorldProjection(glm::vec2(event.width, event.height));
    }

    ~Rigid2DSimlution() {
    }

    void InitString(World& world, const glm::vec2& pos, u32 node, f32 length) {
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


    void InitCircle(World& world, const glm::vec2& pos, i32 n, f32 r, i32 nstep = 1) {
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

    void InitBox(World& mesh, const glm::vec2& pos, f32 l)
    {
        ASSERT(l > 0);
        InitCircle(mesh, pos, 4, sqrt(2.0f) * l * 0.5);
    }


    void InitTriangle(World& mesh, const glm::vec2& pos, f32 l) {
        ASSERT(l > 0);
        InitCircle(mesh, pos, 3, l);
    }

    PointConstraint* AddHorizontalPointConstraint(World& world, const glm::dvec2& pos, f32 r) {
        auto result = world.addConstraint(new PointConstraint(r, [](const f64& dt, Object* self, Object* target) {
                        (void)dt; (void)self; (void)target;
                        if (target) {
                            target->m_pos.y = self->m_pos.y;
                            self->m_pos.x = target->m_pos.x;
                        }
                    }));
        result->self = Object(pos, 0, glm::vec3(COLOR(0x3c4467)));
        return result;
    }

    PointConstraint* AddFixPointConstraint(World& world, const glm::dvec2& pos, f32 r) {
        auto result = world.addConstraint(new PointConstraint(r, [](const f64& dt, Object* self, Object* target) {
                        (void)dt; (void)self; (void)target;
                        if (target) {
                            target->m_pos = self->m_pos;
                        }
                    }));

        result->self = Object(pos, 0, glm::vec3(COLOR(0x486577)));
        return result;
    }

};

namespace Log {
    template <>
    struct Pattern<glm::vec2> {
        static void Log(const glm::vec2& value, const std::string& format) {
            (void)format;
            LOG_INFO("[{}, {}]", value.x, value.y);
        }
    };

}

mfw::Application* mfw::CreateApplication() {
    return new Rigid2DSimlution();
}

