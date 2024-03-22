#include <mfw.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Renderer.h"
#include "Circle.h"
#include "DistanceConstraint.h"
#include "PointConstraint.h"
#include "TestScene.h"

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
    {
    }

    virtual void Start() override {
        update_status();
        GetWindow()->setVSync(true);
    };

    virtual void Update() override {
        static f64 start = Time::GetCurrent();
        update_status();
        update(1.0 / fps); 
        render();
        renderImgui();
        GetWindow()->swapBuffers();
        frame = Time::GetCurrent() - start;
        start += frame;
        // printf("%g\n", frame * 1000.0);
    }

    virtual void update(const f64& dt) = 0;
    virtual void render() = 0;
    virtual void renderImgui() = 0;

    ~SandBox() {
    }

};

class Simluation : public SandBox {
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

    Scene* scene = new TestScene();

public:
    Simluation()
        : mode(Mode::Normal)
    {
        world_scale = 10 * scene->unitScale;
        shift_rate = 0.001 * world_scale;
        zoom_rate = 0.01 * world_scale;
        zoom = 1;
    }

    virtual void Start() override {
        SandBox::Start();
        preview.reserve(16);
        SetWorldProjection(glm::vec2(width, height));

        i32 sub_step = 500;
        bool pause = false,
             gravity = true,
             world_view = true,
             velocity_view = false,
             acceleration_view = false;

        glClearColor(0.1, 0.1, 0.1, 1);
    }

    void SetWorldProjection(glm::vec2 view) {
        glm::vec2 world = world_scale * glm::vec2(view.x, view.y) / view.y;
        scene->camera.ortho = glm::ortho(-world.x, world.x, -world.y, world.y, -1.0f, 1.0f);
    }

    void wall_collision(f64 dt, Circle* c, const glm::vec2& world) {
        static f64 bounce = 0.5;
        glm::vec2 a{}, s{};
        if (c->m_pos.y - c->r < -world.y) {
            s.y = -world.y - c->m_pos.y + c->r;
            a.y = (c->m_velocity.y * (-bounce  - 1)) / dt;
        }
        if (c->m_pos.x - c->r < -world.x) {
            s.x = -world.x - c->m_pos.x + c->r;
            a.x = (c->m_velocity.x * (-bounce - 1)) / dt;
        }
        else if (c->m_pos.x + c->r > world.x) {
            s.x = world.x - c->m_pos.x - c->r;
            a.x = (c->m_velocity.x * (-bounce - 1)) / dt;
        }
        c->m_pos += s;
        c->m_acceleration += a;
    };

    void ApplyUserInputToScene() {
        //glm::dvec2 wpos = MouseToWorldCoord(mouse);
        glm::dvec2 wpos = scene->mouseToWorldCoord(Get()->GetWindow());
        if (holding->getType() == ObjectType::None) {
            holding->m_pos = wpos - (glm::dvec2)catch_offset;
            return;
        }
        f64 k = (f64)settings.sub_step * 80;
        holding->addForce((wpos - (glm::dvec2)catch_offset - holding->m_pos) * k);
    }

    virtual void update(const f64& dt) override {
        Timer timer;
        if (!settings.pause && mode != Mode::Edit) {
            sub_dt = dt / (f64)settings.sub_step;
            for (i32 i = 0; i < settings.sub_step; ++i) {
                scene->world.update(sub_dt);
                for (auto& obj : scene->world.getObjects<Circle>()) {
                    wall_collision(sub_dt, static_cast<Circle*>(obj), scene->world.size);
                }
                scene->update(sub_dt);
            }
            if (holding) {
                ApplyUserInputToScene();
            }
        }

        if ((i32)!Input::KeyPress(' ') & (i32)!Input::KeyPress(VK_CONTROL)) {
            mode = Mode::Normal;
        }
        update_frame = timer.getDuration();
    }

    virtual void render() override {
        Timer timer;
        renderer.clear();

        glm::mat4 proj = scene->camera.getProjection();

        if (settings.world_view) {
            scene->render(renderer);
        }

        auto& unitScale = scene->unitScale;
        if (settings.velocity_view || settings.acceleration_view) {
            auto& objects = scene->world.getObjects<Circle>();
            for (auto& obj : objects) {
                if (settings.acceleration_view) {
                    glm::dvec2 a = (obj->m_velocity - obj->m_ovelocity) / (sub_dt * 30.0);
                    //a = obj->m_acceleration / 20.0;
                    renderer.renderRactangle(proj, obj->m_pos,
                            obj->m_pos + a * (f64)unitScale, blue, 0.02 * unitScale);
                }
                if (settings.velocity_view) {
                    glm::dvec2 v = obj->m_velocity / 6.0;
                    renderer.renderRactangle(proj, obj->m_pos, 
                            obj->m_pos + v * (f64)unitScale, red, 0.02 * unitScale);
                }
            }
        }

        if (holding) {
            const glm::dvec2 m = scene->mouseToWorldCoord(Get()->GetWindow());
            renderer.renderRactangle(proj, m, holding->m_pos, glm::vec3(COLOR(0xb92c2c)), 0.1 * unitScale);
        }

        render_frame = timer.getDuration();
    }

    std::vector<Circle*> FindCirclesByPosition(const glm::vec2& pos) {
        std::vector<Circle*> result;
        for (auto& obj : scene->world.getObjects<Circle>()) {
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

        auto& unitScale = scene->unitScale;
        ImGui::Text("SF:%d", (i32)(1.0 / sub_dt));
        ImGui::Text("ST[ms]:%.2g", 1000.0 * update_frame);
        ImGui::Text("RT[ms]:%.2g", 1000.0 * render_frame);
        ImGui::Text("FPS:%5.1f", 1.0 / frame);
        ImGui::Text("objects:%d", GetObjectsCount());
        ImGui::Text("unit:%gcm", unitScale * 100);
        ImGui::Text("Step:%d", settings.sub_step);

        ImGui::Checkbox("pause", &settings.pause);
        ImGui::Checkbox("gravity", &settings.gravity);
        ImGui::Checkbox("world view", &settings.world_view);
        ImGui::Checkbox("velocity view", &settings.velocity_view);
        ImGui::Checkbox("acceleration view", &settings.acceleration_view);

        bool motified = false;
        auto& attri = scene->attri;
        // motified |= ImGui::SliderFloat("hardness", &attri.hardness, 0.01f, 1.0f);
        // motified |= ImGui::SliderFloat("line width", &attri.line_width, 0.01f * unitScale, 0.3f * unitScale);
        // motified |= ImGui::SliderFloat("node size", &attri.node_size, 0.1f * unitScale, 1 * unitScale);
        // motified |= ImGui::ColorEdit3("node color", glm::value_ptr(attri.node_color));
        //
        // if (motified) {
        //     preview.clear();
        //     for (auto& obj : scene->world.getObjects<Circle>()) {
        //         Circle* circle = static_cast<Circle*>(obj);
        //         circle->m_color = attri.node_color;
        //         circle->r = attri.node_size;
        //     }
        //     for (auto& obj : scene->world.getConstraint<DistanceConstraint>()) {
        //         DistanceConstraint* dc = static_cast<DistanceConstraint*>(obj);
        //         dc->hardness = attri.hardness;
        //     }
        // }

        if (ImGui::Button("restart")) {
            scene->reset();
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
            auto objects = FindCirclesByPosition(scene->mouseToWorldCoord(Get()->GetWindow()));
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

    i32 GetObjectsCount() {
        auto& dc = scene->world.getConstraint<DistanceConstraint>();
        auto& circles = scene->world.getObjects<Circle>();
        return dc.size() + circles.size();
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
                    prev->m_color = scene->attri.node_color;
                }
                mode = Mode::Action;
            }
        }

    }

    virtual void OnCursorMove(const CursorMoveEvent& event) override {
        static glm::vec2 m = glm::vec2(event.x, event.y);
        if (mode == Mode::Action && Input::MouseButtonDown(Left)) {
            scene->camera.view = glm::translate(scene->camera.view, glm::vec3(event.x - m.x, m.y - event.y, 0) * shift_rate);
        }
        m = glm::vec2(event.x, event.y);
    }

    virtual void OnMouseScroll(const MouseScrollEvent& event) override {
        if (mode == Mode::Action) {
            f32 val = 1 + event.ydelta * zoom_rate;
            zoom -= event.ydelta * zoom_rate;
            scene->camera.scale = glm::scale(scene->camera.scale, glm::vec3(val, val, 1));
        }
    }

    PointConstraint* FindPointConstraintByPosition(const glm::vec2& pos) {
        auto& objects = scene->world.getConstraint<PointConstraint>();
        for (auto& obj : objects) {
            auto point = static_cast<PointConstraint*>(obj);
            if (point->d > glm::length((glm::vec2)point->self.m_pos - pos)) {
                return point;
            }
        }
        return nullptr;
    }

    Circle* FindCircleByPosition(const glm::vec2& pos) {
        auto& objects = scene->world.getObjects<Circle>();
        for (auto& obj : objects) {
            auto point = static_cast<Circle*>(obj);
            if (point->r > glm::length((const glm::vec2&)point->m_pos - pos)) {
                return point;
            }
        }
        return nullptr;
    }

    virtual void OnMouseButton(const MouseButtonEvent& event) override {
        glm::dvec2 wpos = scene->mouseToWorldCoord(Get()->GetWindow());

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
        static Circle* preview_node = nullptr;
        static PointConstraint* preview_fix_point = nullptr;
        static glm::dvec2 preview_pos;
        if (event.button == MouseButton::Left && event.mode == KeyMode::Down) {
            preview_node = FindCircleByPosition(wpos);
            preview_fix_point = FindPointConstraintByPosition(wpos);
            if (!preview_node) {
                preview_pos = wpos;
            }
        } else if (event.button == MouseButton::Left && event.mode == KeyMode::Release) {
            Circle* second_node = FindCircleByPosition(wpos);
            PointConstraint* point = FindPointConstraintByPosition(wpos);
            auto& attri = scene->attri;

            if (preview_node && second_node) {
                if (preview_node == second_node) {
                    return;
                }
                f64 d = glm::length(preview_node->m_pos - second_node->m_pos);
                scene->world.addConstraint<DistanceConstraint>(preview_node, second_node, d);
            }
            else if (preview_node && second_node == nullptr) {
                f64 d = glm::length(preview_node->m_pos - wpos);
                auto p = scene->world.addObject<Circle>(wpos, attri.node_color, attri.node_size);
                auto dc = scene->world.addConstraint<DistanceConstraint>(preview_node, p, d);

                p->m_color = attri.node_color;
                p->r = attri.node_size;

                if (point) {
                    point->target = p;
                    dc->d = glm::length(point->self.m_pos - preview_node->m_pos);
                }
            }
            else if (preview_node == nullptr && second_node) {
                f64 d = glm::length(preview_pos - second_node->m_pos);
                auto p = scene->world.addObject<Circle>(preview_pos, attri.node_color, attri.node_size);
                auto dc = scene->world.addConstraint<DistanceConstraint>(p, second_node, d);

                p->m_color = attri.node_color;
                p->r = attri.node_size;

                if (preview_fix_point) {
                    preview_fix_point->target = p;
                    dc->d = glm::length(preview_fix_point->self.m_pos - second_node->m_pos);
                }
            }
            else {
                if (preview_pos == wpos) {
                    scene->world.addObject<Circle>(preview_pos, attri.node_color, attri.node_size);
                    return;
                }
                f32 d = glm::length(preview_pos - wpos);
                if (d < attri.node_size * 2) {
                    return;
                }

                auto p1 = scene->world.addObject<Circle>(preview_pos, attri.node_color, attri.node_size);
                auto p2 = scene->world.addObject<Circle>(wpos, attri.node_color, attri.node_size);
                auto dc = scene->world.addConstraint<DistanceConstraint>(p1, p2, d);

                p1->m_color = attri.node_color;
                p1->r = attri.node_size;
                p2->m_color = attri.node_color;
                p2->r = attri.node_size;

                if (point && preview_fix_point) {
                    point->target = static_cast<Object*>(p1);
                    preview_fix_point->target = static_cast<Object*>(p2);
                    dc->d = glm::length(point->self.m_pos - preview_fix_point->self.m_pos);
                }
                else if (preview_fix_point) {
                    preview_fix_point->target = static_cast<Object*>(p1);
                    dc->d = glm::length(preview_fix_point->self.m_pos - wpos);
                }
                else if (point) {
                    point->target = static_cast<Object*>(p2);
                    //dc->d = (f32)glm::length(point->self.m_pos - preview_node->m_pos);
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

    virtual void OnWindowFocus(const WindowFocusEvent& event) override {
        (void)event;
        mode = Mode::Normal;
    }

    ~Simluation() {
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
    return new Simluation();
}

