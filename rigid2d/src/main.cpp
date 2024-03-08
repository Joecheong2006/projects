#include <mfw.h>
#include "Circle.h"
#include "Stick.h"
#include "Renderer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace mfw;
struct String {
    Stick::Attribute attri;
    String(Stick::Attribute attribute = Stick::attribute)
        : attri(attribute)
    {}

    ~String()
    {
        for (auto& e : entities) {
            delete e;
        }
        for (auto& e : sticks) {
            delete e;
        }
    }

    void init_string(i32 node, f32 d, const glm::vec2& pos = glm::vec2(0))
    {
        ASSERT(node > 0);
        this->node = node;
        this->d = d;
        entities.reserve(node + 1);
        sticks.reserve(node);
        for (f32 i = 0; i < entities.capacity(); i++) {
            entities.emplace_back(new Circle(glm::vec2(0, i * d) + pos, attri.node_color, attri.node_size));
        }
        for (i32 i = 0; i < node - 1; i++) {
            sticks.emplace_back(new Stick(&entities[i]->m_pos, &entities[i + 1]->m_pos, d, attri));
        }
    }

    void init_box(f32 l, const glm::vec2& pos = glm::vec2(0))
    {
        ASSERT(l > 0);
        node = 4;
        d = l;
        f32 lh = l * 0.5;
        entities.reserve(4);
        sticks.reserve(6);

        entities.emplace_back(new Circle(glm::vec2(lh, lh) + pos, attri.node_color, attri.node_size));
        entities.emplace_back(new Circle(glm::vec2(lh, -lh) + pos, attri.node_color, attri.node_size));
        entities.emplace_back(new Circle(glm::vec2(-lh, -lh) + pos, attri.node_color, attri.node_size));
        entities.emplace_back(new Circle(glm::vec2(-lh, lh) + pos, attri.node_color, attri.node_size));

        sticks.emplace_back(new Stick(&entities[0]->m_pos, &entities[1]->m_pos, d, attri));
        sticks.emplace_back(new Stick(&entities[1]->m_pos, &entities[2]->m_pos, d, attri));
        sticks.emplace_back(new Stick(&entities[2]->m_pos, &entities[3]->m_pos, d, attri));
        sticks.emplace_back(new Stick(&entities[3]->m_pos, &entities[0]->m_pos, d, attri));
        sticks.emplace_back(new Stick(&entities[0]->m_pos, &entities[2]->m_pos, d * std::sqrt(2.f), attri));
        sticks.emplace_back(new Stick(&entities[1]->m_pos, &entities[3]->m_pos, d * std::sqrt(2.f), attri));
    }

    void init_triangle(f32 l, const glm::vec2& pos = glm::vec2(0)) {
        node = 3;
        d = sqrt(3.0f) * l;
        entities.reserve(3);
        sticks.reserve(3);
        entities.emplace_back(new Circle(glm::vec2(0, l) + pos, attri.node_color, attri.node_size));
        entities.emplace_back(new Circle(glm::vec2(d, -l) * 0.5f + pos, attri.node_color, attri.node_size));
        entities.emplace_back(new Circle(glm::vec2(-d, -l) * 0.5f + pos, attri.node_color, attri.node_size));

        sticks.emplace_back(new Stick(&entities[0]->m_pos, &entities[1]->m_pos, d, attri));
        sticks.emplace_back(new Stick(&entities[1]->m_pos, &entities[2]->m_pos, d, attri));
        sticks.emplace_back(new Stick(&entities[2]->m_pos, &entities[0]->m_pos, d, attri));
    }

    void update() {
        for (auto& stick : sticks) {
            stick->update();
        }
    }

    void render(const glm::mat4& o) {
        for (auto & stick : sticks) {
            stick->render(o);
        }
    }

    std::vector<Circle*> entities;
    std::vector<Stick*> sticks;
    f32 d;
    i32 node;
};

class FixPoint {
public:
    glm::vec2* holding;
    glm::vec2 pos;
    f32 r = 0.4f;

    FixPoint(const glm::vec2& pos = glm::vec2(0))
        : holding(nullptr), pos(pos)
    {}

    void fix() {
        if (holding) {
            *holding = pos;
        }
    }

    void render(glm::mat4& o) {
        Stick::renderer->draw(o, pos - glm::vec2(r, 0), pos + glm::vec2(r, 0), glm::vec3(COLOR(0xaf7434)), r);
    }

};

class DemoSandBox : public Application {
private:
    glm::mat4 o;
    glm::mat4 view;
    glm::mat4 scale;

    glm::vec2 world = glm::vec2(25, 10);
    f32 world_scale = 60;

    f32 d = 2;
    glm::vec2* holding = nullptr;
    Stick::Attribute attri = Stick::attribute;

    std::vector<String> strings;
    std::vector<FixPoint> fixPoints;

    i32 sub_step = 1;
    bool gravity = true;

public:
    DemoSandBox()
        : view(glm::mat4(1)), scale(view)
    {
        Stick::renderer = new Stick::Renderer();
        Circle::renderer = new Circle::Renderer();

        auto window = GetWindow();
        i32 width = window->width(), height = window->height();
        glm::vec2 world = glm::vec2(width, height) / world_scale;
        o = glm::ortho(-world.x, world.x, -world.y, world.y, -1.0f, 1.0f);

        view = glm::translate(view, glm::vec3(0, 1, 0));

        init_strings();
        init_fixpoint();

        glClearColor(0.1, 0.1, 0.1, 0);
    }

    void init_strings() {
        strings.reserve(4);

        strings.emplace_back();
        strings.back().init_string(3, d * 1.75f, glm::vec2(-1, 2));

        strings.emplace_back();
        strings.back().init_string(5, 2, glm::vec2(2, 4));

        strings.emplace_back();
        strings.back().init_box(3, glm::vec2(-6, 2));

        strings.emplace_back();
        strings.back().init_triangle(2,  glm::vec2(6, 2));
    }

    void init_fixpoint() {
        fixPoints.reserve(2);
        fixPoints.emplace_back(FixPoint(glm::vec2(-3, 1)));
        fixPoints.emplace_back(FixPoint(glm::vec2(3, 1)));
    }

    void update_physics(f32 dt) {
        for (auto& string : strings) {
            for (auto& e : string.entities) {
                if (gravity) {
                    e->add_force(glm::vec2(0, -98.1 * e->m_mass * 1.8));
                }
                e->update(dt);
            }
        }
    }

    void update_collision() {
        if (holding) {
            auto window = GetWindow();
            f32 width = window->width(), height = window->height();
            auto& mouse = Input::GetMouse();
            glm::vec4 uv = glm::vec4(mouse.first / width, 1 - mouse.second / height, 0, 0) * 2.0f - 1.0f;
            glm::vec2 wpos = glm::vec2((view * (uv / o)) / scale);
            *holding = wpos;
        }
        for (auto& point : fixPoints)  {
            point.fix();
            if (point.pos.y - point.r < -world.y) {
                point.pos.y += -world.y - point.pos.y + point.r;
            }
            if (point.pos.x - point.r < -world.x) {
                point.pos.x += -world.x - point.pos.x + point.r;
            }
            else if (point.pos.x + point.r > world.x) {
                point.pos.x += world.x - point.pos.x - point.r;
            }
        }
        for (auto& string : strings) {
            for (auto& e : string.entities) {
                if (e->m_pos.y - e->r < -world.y) {
                    e->m_pos.y += -world.y - e->m_pos.y + e->r;
                }
                if (e->m_pos.x - e->r < -world.x) {
                    e->m_pos.x += -world.x - e->m_pos.x + e->r;
                }
                else if (e->m_pos.x + e->r > world.x) {
                    e->m_pos.x += world.x - e->m_pos.x - e->r;
                }
            }
        }
    }

    void update_constraint() {
        for (auto& string : strings) {
            string.update();
        }
    }

    virtual void Update() override {
        static f32 frame = 1.0 / 144;

        glClear(GL_COLOR_BUFFER_BIT);

        for (i32 i = 0; i < sub_step; i++) {
            update_physics(frame / sub_step);
            update_collision();
            update_constraint();
        }

        glm::mat4 proj = o * view * scale;

        Stick::renderer->bind();
        for (auto& point : fixPoints)  {
            point.render(proj);
        }
        Stick::renderer->unbind();

        for (auto& string : strings) {
            string.render(proj);
        }

        Stick::renderer->bind();
        Stick::renderer->draw(proj, glm::vec2(world.x, -world.y) - 0.2f, glm::vec2(-world.x, -world.y) - 0.2f, glm::vec3(1), 0.2);
        Stick::renderer->unbind();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("config");

        ImGui::Checkbox("gravity", &gravity);
        ImGui::SliderInt("sub step", &sub_step, 1, 100);

        bool motified = false;
        motified |= ImGui::SliderFloat("line width", &attri.line_width, 0.02f, 0.3f);
        motified |= ImGui::SliderFloat("bounce", &attri.bounce, 0.0f, 1.0f);
        motified |= ImGui::SliderFloat("node size", &attri.node_size, 0.1f, 0.5f);
        motified |= ImGui::ColorEdit3("node color", glm::value_ptr(attri.node_color));

        if (motified) {
            for (auto& string : strings) {
                for (auto& stick : string.sticks) {
                    stick->attri = attri;
                }
                for (auto& e : string.entities) {
                    e->r = attri.node_size;
                }
            }
        }

        if (ImGui::Button("restart")) {
            strings.clear();
            init_strings();
            fixPoints.clear();
            init_fixpoint();
            attri = Stick::attribute;
            gravity = true;
            sub_step = 1;
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    virtual void OnInputKey(const KeyEvent& event) override {
        if (event.key == VK_ESCAPE && event.mode == KeyMode::Down) {
            Terminate();
        }
    }

    virtual void OnCursorMove(const CursorMoveEvent& event) override {
        static i32 mx = event.x, my = event.y;
        if (!holding && Input::KeyPress(VK_CONTROL)) {
            if (Input::MouseButtonDown(Left)) {
                view = glm::translate(view, glm::vec3(event.x - mx, my - event.y, 0) * 0.025f);
            }
            mx = event.x, my = event.y;
        }
    }

    virtual void OnMouseScroll(const MouseScrollEvent& event) override {
        if (Input::KeyPress(VK_CONTROL)) {
            f32 val = 1 - event.ydelta * 0.04f;
            scale = glm::scale(scale, glm::vec3(val, val, 1));
        }
    }

    Circle* find_circle_by_position(glm::vec2 pos) {
        for (auto& string : strings) {
            for (i32 i = string.entities.size() - 1; i >= 0; i--) {
                if (string.entities[i]->r > glm::length(string.entities[i]->m_pos - pos)) {
                    return string.entities[i];
                }
            }
        }
        return nullptr;
    }

    FixPoint* find_fix_point_by_position(glm::vec2 pos) {
        for (auto& point : fixPoints) {
            if (point.r > glm::length(point.pos - pos)) {
                return &point;
            }
        }
        return nullptr;
    }

    virtual void OnMouseButton(const MouseButtonEvent& event) override {
        auto window = GetWindow();
        f32 width = window->width(), height = window->height();
        auto& mouse = Input::GetMouse();
        glm::vec4 uv = glm::vec4(mouse.first / width, 1 - mouse.second / height, 0, 0) * 2.0f - 1.0f;
        glm::vec2 wpos = glm::vec2((view * (uv / o)) / scale);

        if (event.button == MouseButton::Left && event.mode == KeyMode::Release) {
            if (holding) {
                FixPoint* fix = find_fix_point_by_position(*holding);
                if (fix) {
                    fix->holding = holding;
                    holding = nullptr;
                }
            }
        }

        if (event.button == MouseButton::Left && event.mode == KeyMode::Down) {
            if (!holding) {
                FixPoint* fix = find_fix_point_by_position(wpos);
                if (fix) {
                    holding = fix->holding;
                    fix->holding = nullptr;
                }
            } 
        }

        if (!Input::KeyPress(VK_CONTROL) && event.button == MouseButton::Left && event.mode == KeyMode::Down) {
            if (!holding) {
                Circle* circle = find_circle_by_position(wpos);
                if (circle) {
                    holding = &circle->m_pos;
                }
            }
        } else {
            holding = nullptr;
        }

        if (Input::MouseButtonDown(MouseButton::Right)) {
            if (!holding) {
                FixPoint* fix = find_fix_point_by_position(wpos);
                if (fix) {
                    holding = &fix->pos;
                }
            }
        }
    }

    virtual void OnWindowResize(const WindowResizeEvent& event) override {
        glViewport(0, 0, event.width, event.height);
        glm::vec2 world = glm::vec2(event.width, event.height) / world_scale;
        o = glm::ortho(-world.x, world.x, -world.y, world.y, -1.0f, 1.0f);
    }

    ~DemoSandBox() {
        delete Stick::renderer;
        delete Circle::renderer;
    }

};

mfw::Application* mfw::CreateApplication() {
    return new DemoSandBox();
}

