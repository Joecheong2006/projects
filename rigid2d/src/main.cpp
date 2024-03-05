#include <mfw.h>
#include "Circle.h"
#include "glm/gtc/matrix_transform.hpp"

struct Stick {
    Stick() {}
    Stick(glm::vec2* p1, glm::vec2* p2, f32 d)
        : d(d)
    {
        p[0] = p1;
        p[1] = p2;
    }

    void solve_constraint() {
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

using namespace mfw;
class DemoSandBox : public Application {
private:
    glm::mat4 o;
    glm::vec2 ws;
    Circle::Manager cm;

    Stick stick[2];
    f32 r = 2;

public:
    DemoSandBox()
    {
        auto window = GetWindow();
        ws = glm::vec2(window->width(), window->height()) / 60.f;
        o = glm::ortho(-ws.x, ws.x, -ws.y, ws.y, -1.0f, 1.0f);

        cm.createCircle(glm::vec2(0, 0), glm::vec3(1), 0.3);
        cm.createCircle(glm::vec2(0, r), glm::vec3(0.6), 0.3);
        cm.createCircle(glm::vec2(0, 2 * r), glm::vec3(0.3), 0.3);
        stick[0] = Stick(&cm.entities[0].m_pos,
                         &cm.entities[1].m_pos, r);
        stick[1] = Stick(&cm.entities[1].m_pos,
                         &cm.entities[2].m_pos, r);

        glClearColor(0.1, 0.1, 0.1, 0);
    }

    virtual void Update() override {
        f32 frame = 1.0 / 144;

        auto window = GetWindow();
        f32 width = window->width(), height = window->height();
        if (Input::MouseButtonDown(Left)) {
            f32 mouse_x = Input::GetMouse().first, mouse_y = Input::GetMouse().second;
            cm.entities[0].m_pos = glm::vec2(mouse_x / 30 - width / 60, height / 60 - mouse_y / 30);
        }

        glClear(GL_COLOR_BUFFER_BIT);

        for (auto& e : cm.entities) {
            e.add_force(glm::vec2(0, -98.1));
            e.update(frame);
        }

        for (auto& e : cm.entities) {
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

        stick[0].solve_constraint();
        stick[1].solve_constraint();

        cm.renderCircle(o);

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
        stick[0].d -= event.ydelta * 0.1f;
        stick[1].d -= event.ydelta * 0.1f;
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
