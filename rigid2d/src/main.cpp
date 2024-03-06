#include <mfw.h>
#include "Circle.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"
#include "glm/gtc/matrix_transform.hpp"

using namespace mfw;

struct Stick {
    Stick() {}
    Stick(glm::vec2* p1, glm::vec2* p2, f32 d)
        : d(d)
    {
        p[0] = p1;
        p[1] = p2;
    }

    void update() {
        f32 cd = glm::length(*p[0] - *p[1]);
        if (cd == d)
            return;
        glm::vec2 nd = glm::normalize(*p[0] - *p[1]) * (d - cd) * 0.5f;
        *p[0] += nd;
        *p[1] -= nd;
    }

    f32 d;
    glm::vec2* p[2];

};

struct String {
    String()
    {}

    void init_string(i32 node, i32 d)
    {
        ASSERT(node > 0);
        this->node = node;
        this->d = d;
        entities.reserve(node + 1);
        sticks.reserve(node);
        for (i32 i = 0; i < node; i++) {
            entities.emplace_back(glm::vec2(0, i * d), glm::vec3((i + 1.0f) / node), 0.4);
        }
        for (i32 i = 0; i < node - 1; i++) {
            sticks.emplace_back(&entities[i].m_pos, &entities[i + 1].m_pos, d);
        }
    }

    void init_box(f32 l)
    {
        ASSERT(l > 0);
        this->node = 4;
        this->d = l;
        f32 lh = l * 0.5;
        entities.reserve(4);
        sticks.reserve(6);

        entities.emplace_back(glm::vec2(lh, lh), glm::vec3(1), 0.4);
        entities.emplace_back(glm::vec2(lh, -lh), glm::vec3(1), 0.4);
        entities.emplace_back(glm::vec2(-lh, -lh), glm::vec3(1), 0.4);
        entities.emplace_back(glm::vec2(-lh, lh), glm::vec3(1), 0.4);

        sticks.emplace_back(&entities[0].m_pos, &entities[1].m_pos, d);
        sticks.emplace_back(&entities[1].m_pos, &entities[2].m_pos, d);
        sticks.emplace_back(&entities[2].m_pos, &entities[3].m_pos, d);
        sticks.emplace_back(&entities[3].m_pos, &entities[0].m_pos, d);
        sticks.emplace_back(&entities[0].m_pos, &entities[2].m_pos, d * std::sqrt(2.f));
        sticks.emplace_back(&entities[1].m_pos, &entities[3].m_pos, d * std::sqrt(2.f));
    }

    void init_triangle(f32 l) {
        this->node = 3;
        this->d = l;
    }

    void update() {
        for (auto& stick : sticks) {
            stick.update();
        }
    }

    std::vector<Circle> entities;
    std::vector<Stick> sticks;
    i32 node, d;
};

class DemoSandBox : public Application {
private:
    glm::mat4 o;
    glm::vec2 ws;
    Circle::Manager cm;

    f32 d = 2, r = 0.4;
    std::vector<String> strings;
    Circle* holding = nullptr;

public:
    DemoSandBox()
    {
        auto window = GetWindow();
        ws = glm::vec2(window->width(), window->height()) / 60.f;
        o = glm::ortho(-ws.x, ws.x, -ws.y, ws.y, -1.0f, 1.0f);

        strings.push_back(String());
        strings.back().init_string(3, d);
        strings.push_back(String());
        strings.back().init_box(3);
        glClearColor(0.1, 0.1, 0.1, 0);
    }

    virtual void Update() override {
        f32 frame = 1.0 / 144;

        auto window = GetWindow();
        f32 width = window->width(), height = window->height();
        if (Input::MouseButtonDown(Left)) {
            auto& mouse = Input::GetMouse();
            glm::vec2 pos = glm::vec2(mouse.first / 30.0f - width / 60, height / 60 - mouse.second / 30.0f);
            if (holding) {
                holding->m_pos = pos;
            } else {
                for (auto& string : strings) {
                    for (i32 i = string.entities.size() - 1; i >= 0; i--) {
                        if (r > glm::length(string.entities[i].m_pos - pos)) {
                            holding = &string.entities[i];
                            break;
                        }
                    }
                }
            }
        } else {
            holding = nullptr;
        }

        glClear(GL_COLOR_BUFFER_BIT);

        for (auto& string : strings) {
            for (auto& e : string.entities) {
                e.add_force(glm::vec2(0, -98.1));
                e.update(frame);
            }
        }

        for (auto& string : strings) {
            for (auto& e : string.entities) {
                if (e.m_pos.y - e.d < -ws.y) {
                    e.m_pos.y += -ws.y - e.m_pos.y + e.d;
                }
                if (e.m_pos.x - e.d < -ws.x) {
                    e.m_pos.x += -ws.x - e.m_pos.x + e.d;
                }
                if (e.m_pos.x + e.d > ws.x) {
                    e.m_pos.x += ws.x - e.m_pos.x - e.d;
                }
            }
        }

        for (auto& string : strings) {
            string.update();
        }

        for (auto& string : strings) {
            for (auto & e : string.entities) {
                cm.renderCircle(o, e);
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("status");
        ImGui::Text("hello, world\n");
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    virtual void OnInputKey(const KeyEvent& event) override {
        if (event.key == VK_ESCAPE && event.mode == KeyMode::Down) {
            Terminate();
        }
    }

    virtual void OnMouseScroll(const MouseScrollEvent& event) override {
        (void)event;
    }

    virtual void OnWindowResize(const WindowResizeEvent& event) override {
        glViewport(0, 0, event.width, event.height);
        glm::vec2 ws = glm::vec2(event.width, event.height) / 60.f;
        o = glm::ortho(-ws.x, ws.x, -ws.y, ws.y, -1.0f, 1.0f);
    }

    ~DemoSandBox() {
    }

};

mfw::Application* mfw::CreateApplication() {
    return new DemoSandBox();
}

