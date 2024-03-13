#include <mfw.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Circle.h"
#include "Stick.h"
#include "FixPoint.h"
#include "Mesh.h"

#include "Renderer.h"

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
    const f32 refresh_rate = 144, fps = refresh_rate;
    f32 refresh_frame = 0;

    SandBox(): Application("rigid2d", 1104, 720)
    {}

    virtual void Start() override {
        Get()->GetWindow()->setVSync(false);
        Stick::renderer = new Stick::Renderer();
        Circle::renderer = new Circle::Renderer();
        update_status();
    };

    virtual void Update() override {
        static f32 start = Time::GetCurrent(), end = 0;
        update_status();
        update(1.0 / fps); 
        render();
        update_input();
        end = Time::GetCurrent() - start;
        if (end < 1.0 / refresh_rate) {
            Time::Sleep((1.0 / refresh_rate - end) * 1000);
        }
        refresh_frame = Time::GetCurrent() - start;
        start = Time::GetCurrent();
    }

    virtual void update(const f64& dt) = 0;
    virtual void render() = 0;
    virtual void update_input() = 0;

    ~SandBox() {
        delete Stick::renderer;
        delete Circle::renderer;
    }

};

class Rigid2DSimlution : public SandBox {
private:
    struct Settings {
        i32 sub_step = 5;
        f32 time_rate = 1.0;
        i32 gravity_factor = 2;
        bool gravity = true, world_view = true, velocity_view = false;
    } settings;

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

    // world
    glm::vec2 world = glm::vec2(5, 2);
    f32 world_scale = 2.4;
    f32 shift_rate = 0.002 * world_scale;
    f32 zoom_rate = 0.01 * world_scale;
    glm::mat4 worldProjection;

    std::vector<Mesh*> meshes;
    std::vector<FixPoint*> fixPoints;

    Object2D* holding = nullptr;
    std::vector<Object2D*> preview;
    Stick::Attribute attri;

    Circle* preview_node = nullptr;
    FixPoint* preview_fix_point = nullptr;
    glm::dvec2 preview_pos;

    Mode mode;
    f64 sub_dt;
    f32 unitScale = 0.1f;

public:
    Rigid2DSimlution()
        : o(glm::mat4(1)), view(o), scale(o), mode(Mode::Normal)
    {}

    virtual void Start() override {
        SandBox::Start();

        SetWorldProjection(glm::vec2(width, height));
        o = worldProjection;

        preview.reserve(16);

        SetDefaultStickAttribute();
        InitializeFixPoints();
        InitializeMeshes();

        glClearColor(0.1, 0.1, 0.1, 1);
    }

    void SetDefaultStickAttribute() {
        attri.node_color = glm::vec4(glm::vec4(COLOR(0x858AA6), 0));
        attri.node_size = 0.6 * unitScale;
        attri.line_width = 0.16 * unitScale;
        attri.hardness = 1;
    }

    void InitializeMeshes() {
        meshes.push_back(new Mesh(attri));

        meshes.push_back(new Mesh(attri));
        InitString(meshes.back(), glm::vec2(-2, 4) * unitScale, 7 * unitScale, 3);

        meshes.push_back(new Mesh(attri));
        InitBox(meshes.back(), glm::vec2(-4, 0) * unitScale, 7 * unitScale);
        meshes.push_back(new Mesh(attri));
        InitBox(meshes.back(), glm::vec2(4, 0) * unitScale, 7 * unitScale);

        meshes.push_back(new Mesh(attri));
        InitTriangle(meshes.back(), glm::vec2(0), 5 * unitScale);
        meshes.push_back(new Mesh(attri));
        InitTriangle(meshes.back(), glm::vec2(0), 5 * unitScale);
    }

    void FreeMeshes() {
        for (auto& mesh : meshes){
            delete mesh;
        }
        meshes.clear();
    }

    void InitializeFixPoints() {
        for (i32 i = 0; i < 10; i++) {
            fixPoints.push_back(new FixPoint(8 * unitScale, glm::vec2(0, 8) * unitScale));
        }
    }

    void FreeFixPoints() {
        for (auto& fixPoint : fixPoints){
            delete fixPoint;
        }
        fixPoints.clear();
    }

    void SetWorldProjection(glm::vec2 view) {
        glm::vec2 world = world_scale * glm::vec2(view.x, view.y) / view.y;
        worldProjection = glm::ortho(-world.x, world.x, -world.y, world.y, -1.0f, 1.0f);
    }

    glm::dvec2 MouseToWorldCoord(glm::vec2 mouse) {
        glm::vec4 uv = glm::vec4(mouse.x / width, 1 - mouse.y / height, 0, 0) * 2.0f - 1.0f;
        return view * ((uv / o) / scale);
    }

    void update_physics() {
        for (auto& mesh : meshes) {
            for (auto& e : mesh->entities) {
                e->addForce(glm::vec2(0, -9.81f * e->m_mass * settings.gravity * settings.gravity_factor));
                e->update(sub_dt);
            }
        }
    }

    void wall_collision(Circle* c, const glm::vec2& world) {
        if (c->m_pos.y - c->r < -world.y) {
            f32 ay = ((-world.y - c->m_pos.y + c->r) - (c->m_pos.y - c->m_opos.y)) / (sub_dt * sub_dt);
            c->addForce(glm::vec2(0, ay * c->m_mass));
        }
        if (c->m_pos.x - c->r < -world.x) {
            f32 ax = ((-world.x - c->m_pos.x + c->r) - (c->m_pos.x - c->m_opos.x)) / (sub_dt * sub_dt);
            c->addForce(glm::vec2(ax * c->m_mass, 0));
        }
        else if (c->m_pos.x + c->r > world.x) {
            f32 ax = ((world.x - c->m_pos.x - c->r) - (c->m_pos.x - c->m_opos.x)) / (sub_dt * sub_dt);
            c->addForce(glm::vec2(ax * c->m_mass, 0));
        }
    };

    void update_collision() {
        for (auto& mesh : meshes) {
            for (auto& e : mesh->entities) {
                wall_collision(e, world);
            }
        }
    }

    void update_constraint() {
        if (holding) {
            holding->m_pos = MouseToWorldCoord(mouse);
        }
        for (auto& point : fixPoints)  {
            point->fix();
        }
        for (auto& mesh : meshes) {
            for (auto& stick : mesh->sticks) {
                stick->update(sub_dt);
            }
        }
    }

    virtual void update(const f64& dt) override {
        if (mode != Mode::Edit) {
            sub_dt = dt * (f64)settings.time_rate / settings.sub_step;
            for (i32 i = 0; i < settings.sub_step; i++) {
                update_physics();
                update_collision();
                update_constraint();
            }
        }

        if (!Input::KeyPress(' ') && !Input::KeyPress(VK_CONTROL)) {
            mode = Mode::Normal;
        }
    }

    virtual void render() override {
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 proj = o * scale * view;

        if (settings.world_view) {
            for (auto& fixPoint : fixPoints) {
                fixPoint->render(proj);
            }

            for (auto& mesh : meshes) {
                for (auto& stick : mesh->sticks) {
                    stick->render(proj);
                }
            }

            Stick::renderer->draw(proj, glm::vec2(world.x, -world.y) - 0.2f * unitScale, glm::vec2(-world.x, -world.y) - 0.2f * unitScale, glm::vec4(1), 0.2 * unitScale);
        }

        if (settings.velocity_view) {
            for (auto& mesh : meshes) {
                for (auto& e : mesh->entities) {
                    glm::dvec2 v = (e->m_pos - e->m_opos) / sub_dt;
                    Stick::renderer->draw(proj, e->m_pos, e->m_pos + v * 0.06, red, 0.05 * unitScale);
                }
            }
        }

    }

    virtual void update_input() override {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("config");

        ImGui::Text("update fps: %-5.2f", 1.0f / sub_dt);
        ImGui::Text("refresh fps: %-5.2f", 1.0 / refresh_frame);

        ImGui::Checkbox("gravity", &settings.gravity);
        ImGui::Checkbox("world view", &settings.world_view);
        ImGui::Checkbox("velocity view", &settings.velocity_view);
        ImGui::SliderInt("gravity factor", &settings.gravity_factor, 1, 5);
        ImGui::SliderFloat("time rate", &settings.time_rate, 0, 1);
        ImGui::SliderInt("sub step", &settings.sub_step, 1, 100);

        bool motified = false;
        motified |= ImGui::SliderFloat("hardness", &attri.hardness, 0.1f, 1.0f);
        motified |= ImGui::SliderFloat("line width", &attri.line_width, 0.01f * unitScale, 0.3f * unitScale);
        motified |= ImGui::SliderFloat("node size", &attri.node_size, 0.1f * unitScale, 1 * unitScale);
        motified |= ImGui::ColorEdit3("node color", glm::value_ptr(attri.node_color));

        if (motified) {
            preview.clear();
            for (auto& mesh : meshes) {
                for (auto& stick : mesh->sticks) {
                    stick->setAttribute(attri);
                }
            }
        }

        ImGui::Text("sticks: %d", GetSticksCount());

        if (ImGui::Button("restart")) {
            FreeMeshes();
            InitializeMeshes();
            FreeFixPoints();
            InitializeFixPoints();
            SetDefaultStickAttribute();
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
            glm::vec2 wpos = MouseToWorldCoord(mouse);
            auto circles = FindCirclesByPosition(wpos);
            for (auto& prev : preview) {
                prev->m_color = attri.node_color;
            }
            if (!circles.empty()) {
                for (auto& circle : circles) {
                    preview.emplace_back(circle);
                }
                for (auto& circle : circles) {
                    circle->m_color = glm::vec4(COLOR(0x5D627E), 0);
                }
            }
        }
    }

    i32 GetSticksCount() {
        i32 result = 0;
        for (auto& mesh : meshes) {
            result += mesh->sticks.size();
        }
        return result;
    }

    virtual void OnInputKey(const KeyEvent& event) override {
        if (event.key == VK_ESCAPE && event.mode == KeyMode::Down) {
            Terminate();
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

    std::vector<Circle*> FindCirclesByPosition(const glm::vec2& pos) {
        std::vector<Circle*> result;
        for (auto& mesh : meshes) {
            for (auto& e : mesh->entities) {
                if (e->r > glm::length((const glm::vec2&)e->m_pos - pos)) {
                    result.push_back(e);
                }
            }
        }
        return result;
    }

    Circle* FindCircleByPosition(const glm::vec2& pos) {
        for (auto& mesh : meshes) {
            for (auto& e : mesh->entities) {
                if (e->r > glm::length((const glm::vec2&)e->m_pos - pos)) {
                    return e;
                }
            }
        }
        return nullptr;
    }

    FixPoint* FindFixPointByPosition(const glm::vec2& pos) {
        for (auto& point : fixPoints) {
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
            preview_fix_point = FindFixPointByPosition(wpos);
            if (!preview_node) {
                preview_pos = wpos;
            }
        } else if (event.button == MouseButton::Left && event.mode == KeyMode::Release) {
            Circle* second_node = FindCircleByPosition(wpos);
            FixPoint* fixPoint = FindFixPointByPosition(wpos);

            if (preview_node && second_node) {
                if (preview_node == second_node) {
                    return;
                }
                f64 d = glm::length(preview_node->m_pos - second_node->m_pos);
                meshes[0]->sticks.push_back(new Stick(preview_node, second_node, d, attri));
            }
            else if (preview_node && second_node == nullptr) {
                f64 d = glm::length(preview_node->m_pos - wpos);
                meshes[0]->entities.push_back(new Circle(wpos, attri.node_color, attri.node_size));
                meshes[0]->sticks.push_back(new Stick(preview_node, meshes[0]->entities.back(), d, attri));
                if (fixPoint) {
                    fixPoint->holding = static_cast<Object2D*>(meshes[0]->entities.back());
                    meshes[0]->sticks.back()->d = glm::length(fixPoint->m_pos - preview_node->m_pos);
                }
            }
            else if (preview_node == nullptr && second_node) {
                f64 d = glm::length(preview_pos - second_node->m_pos);
                meshes[0]->entities.push_back(new Circle(preview_pos, attri.node_color, attri.node_size));
                meshes[0]->sticks.push_back(new Stick(meshes[0]->entities.back(), second_node, d, attri));
                if (preview_fix_point) {
                    preview_fix_point->holding = static_cast<Object2D*>(meshes[0]->entities.back());
                    meshes[0]->sticks.back()->d = glm::length(preview_fix_point->m_pos - second_node->m_pos);
                }
            }
            else {
                if (preview_pos == wpos) {
                    return;
                }
                f32 d = glm::length(preview_pos - wpos);
                if (d < attri.node_size * 2) {
                    return;
                }
                auto first = new Circle(preview_pos, attri.node_color, attri.node_size);
                meshes[0]->entities.push_back(first);
                auto second = new Circle(wpos, attri.node_color, attri.node_size);
                meshes[0]->entities.push_back(second);
                meshes[0]->sticks.push_back(new Stick(first, second, d, attri));
                if (fixPoint && preview_fix_point) {
                    fixPoint->holding = static_cast<Object2D*>(second);
                    preview_fix_point->holding = static_cast<Object2D*>(first);
                    meshes[0]->sticks.back()->d = glm::length(fixPoint->m_pos - preview_fix_point->m_pos);
                }
                else if (preview_fix_point) {
                    preview_fix_point->holding = static_cast<Object2D*>(first);
                    meshes[0]->sticks.back()->d = glm::length(preview_fix_point->m_pos - wpos);
                }
                else if (fixPoint) {
                    fixPoint->holding = static_cast<Object2D*>(second);
                    meshes[0]->sticks.back()->d = glm::length(fixPoint->m_pos - preview_node->m_pos);
                }
            }

        }
    }

    void OnNormal(const MouseButtonEvent& event, const glm::vec2& wpos) {
        if (event.button == MouseButton::Left && event.mode == KeyMode::Release) {
            if (holding) {
                FixPoint* fix = FindFixPointByPosition(holding->m_pos);
                if (fix) {
                    fix->holding = holding;
                    holding = nullptr;
                }
            }
        }

        if (event.button == MouseButton::Left && event.mode == KeyMode::Down) {
            if (!holding) {
                FixPoint* fix = FindFixPointByPosition(wpos);
                if (fix) {
                    holding = fix->holding;
                    fix->holding = nullptr;
                }
            }
        }

        if (event.button == MouseButton::Left && event.mode == KeyMode::Down) {
            if (!holding) {
                holding = FindCircleByPosition(wpos);
            }
        } else {
            holding = nullptr;
        }

        if (event.button == MouseButton::Right && event.mode == KeyMode::Down) {
            if (!holding) {
                holding = FindFixPointByPosition(wpos);
            }
        }
    }

    virtual void OnWindowResize(const WindowResizeEvent& event) override {
        glViewport(0, 0, event.width, event.height);
        SetWorldProjection(glm::vec2(event.width, event.height));
        o = worldProjection;
    }

    ~Rigid2DSimlution() {
        FreeMeshes();
        FreeFixPoints();
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

