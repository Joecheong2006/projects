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
    Stick::Attribute& attri;
    String(Stick::Attribute& attribute)
        : attri(attribute)
    {}

    void init_string(i32 node, i32 d, const glm::vec2& pos = glm::vec2(0))
    {
        ASSERT(node > 0);
        this->node = node;
        this->d = d;
        entities.reserve(node + 1);
        sticks.reserve(node);
        for (i32 i = 0; i < node; i++) {
            entities.emplace_back(glm::vec2(0, i * d) + pos, glm::vec3((i + 1.0f) / node), attri.node_size);
        }
        for (i32 i = 0; i < node - 1; i++) {
            sticks.emplace_back(&entities[i].m_pos, &entities[i + 1].m_pos, d, attri);
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

        entities.emplace_back(glm::vec2(lh, lh) + pos, attri.node_color, attri.node_size);
        entities.emplace_back(glm::vec2(lh, -lh) + pos, attri.node_color, attri.node_size);
        entities.emplace_back(glm::vec2(-lh, -lh) + pos, attri.node_color, attri.node_size);
        entities.emplace_back(glm::vec2(-lh, lh) + pos, attri.node_color, attri.node_size);

        sticks.emplace_back(&entities[0].m_pos, &entities[1].m_pos, d, attri);
        sticks.emplace_back(&entities[1].m_pos, &entities[2].m_pos, d, attri);
        sticks.emplace_back(&entities[2].m_pos, &entities[3].m_pos, d, attri);
        sticks.emplace_back(&entities[3].m_pos, &entities[0].m_pos, d, attri);
        sticks.emplace_back(&entities[0].m_pos, &entities[2].m_pos, d * std::sqrt(2.f), attri);
        sticks.emplace_back(&entities[1].m_pos, &entities[3].m_pos, d * std::sqrt(2.f), attri);
    }

    void init_triangle(f32 l, const glm::vec2& pos = glm::vec2(0)) {
        node = 3;
        d = sqrt(3.0f) * l;
        entities.reserve(3);
        sticks.reserve(3);
        entities.emplace_back(glm::vec2(0, l) + pos, attri.node_color, attri.node_size);
        entities.emplace_back(glm::vec2(d, -l) * 0.5f + pos, attri.node_color, attri.node_size);
        entities.emplace_back(glm::vec2(-d, -l) * 0.5f + pos, attri.node_color, attri.node_size);

        sticks.emplace_back(&entities[0].m_pos, &entities[1].m_pos, d, attri);
        sticks.emplace_back(&entities[1].m_pos, &entities[2].m_pos, d, attri);
        sticks.emplace_back(&entities[2].m_pos, &entities[0].m_pos, d, attri);
    }

    void update() {
        for (auto& stick : sticks) {
            stick.update();
        }
    }

    void render(const glm::mat4& o) {
        for (auto & stick : sticks) {
            stick.render(o);
        }
    }

    std::vector<Circle> entities;
    std::vector<Stick> sticks;
    i32 node, d;
};

class FixPoint {
public:
    glm::vec2 pos;
    Circle* holding;
    f32 r = 0.35f;

    FixPoint() 
        : pos(), holding(nullptr)
    {}

    void fix() {
        if (holding) {
            holding->m_pos = pos;
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

    glm::vec2 world;
    f32 world_scale = 60;

    f32 d = 2;
    std::vector<String> strings;
    Circle* holding = nullptr;
    Stick::Attribute attri;
    FixPoint fix_point;

    i32 sub_step = 1;

public:
    DemoSandBox()
    {
        Stick::renderer = new Stick::Renderer();
        Circle::renderer = new Circle::Renderer();

        auto window = GetWindow();
        i32 width = window->width(), height = window->height();
        world = glm::vec2(width, height) / world_scale;
        o = glm::ortho(-world.x, world.x, -world.y, world.y, -1.0f, 1.0f);
        view = glm::mat4(1);
        scale = glm::mat4(1);

        strings.push_back(String(attri));
        strings.back().init_string(3, d * 1.5f);
        
        strings.push_back(String(attri));
        strings.back().init_box(3, glm::vec2(-6, 0));

        strings.push_back(String(attri));
        strings.back().init_triangle(2,  glm::vec2(6, 0));

        glClearColor(0.1, 0.1, 0.1, 0);
    }

    virtual void Update() override {
        static f32 frame = 1.0 / 144;

        glClear(GL_COLOR_BUFFER_BIT);

        for (i32 i = 0; i < sub_step; i++) {
            for (auto& string : strings) {
                for (auto& e : string.entities) {
                    e.add_force(glm::vec2(0, -98.1 * e.m_mass * 2));
                    e.update(frame / (f32)sub_step);
                }
            }

            if (holding) {
                auto window = GetWindow();
                f32 width = window->width(), height = window->height();
                auto& mouse = Input::GetMouse();
                glm::vec4 uv = glm::vec4(mouse.first / width, 1 - mouse.second / height, 0, 0) * 2.0f - 1.0f;
                glm::vec2 wpos = glm::vec2((view * (uv / o)) / scale);
                holding->m_pos = wpos;
            }

            fix_point.fix();

            for (auto& string : strings) {
                for (auto& e : string.entities) {
                    if (e.m_pos.y - e.d < -world.y) {
                        e.m_pos.y += -world.y - e.m_pos.y + e.d;
                    }
                    if (e.m_pos.x - e.d < -world.x) {
                        e.m_pos.x += -world.x - e.m_pos.x + e.d;
                    }
                    if (e.m_pos.x + e.d > world.x) {
                        e.m_pos.x += world.x - e.m_pos.x - e.d;
                    }
                }
            }

            for (auto& string : strings) {
                string.update();
            }
        }

        // --------

        glm::mat4 proj = o * view * scale;

        fix_point.render(proj);

        for (auto& string : strings) {
            string.render(proj);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("config");

        ImGui::SliderFloat("line width", &attri.line_width, 0.02f, 0.3f);
        ImGui::SliderFloat("bounce", &attri.bounce, 0.0f, 1.0f);
        if (ImGui::SliderFloat("node size", &attri.node_size, 0.1f, 0.5f)) {
            for (auto& string : strings) {
                for (auto& e : string.entities) {
                    e.d = attri.node_size;
                }
            }
        }
        ImGui::ColorEdit3("node color", glm::value_ptr(attri.node_color));
        ImGui::SliderInt("sub step", &sub_step, 1, 100);

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
                if (attri.node_size > glm::length(string.entities[i].m_pos - pos)) {
                    return &string.entities[i];
                }
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

        if (event.button == MouseButton::Left && event.mode == KeyMode::Down) {
            if (!holding) {
                if (glm::length(wpos - fix_point.pos) < fix_point.r) {
                    holding = fix_point.holding;
                    fix_point.holding = nullptr;
                }
            }
        }
        if (event.button == MouseButton::Left && event.mode == KeyMode::Release) {
            if (holding) {
                if (glm::length(holding->m_pos - fix_point.pos) < holding->d + fix_point.r) {
                    fix_point.holding = holding;
                    holding = nullptr;
                }
            }
        }

        if (!Input::KeyPress(VK_CONTROL) && event.button == MouseButton::Left && event.mode == KeyMode::Down) {
            if (!holding) {
                holding = find_circle_by_position(wpos);
            }
        } else {
            holding = nullptr;
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

