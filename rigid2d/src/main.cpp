#include <mfw.h>
#include "Circle.h"
#include "Renderer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define COLOR(val) ((u32)val >> 24) / 255.0, (((u32)val << 8) >> 24) / 255.0, (((u32)val << 16) >> 24) / 255.0

static f32 r = 0.3f;
static glm::vec3 node_color = glm::vec3(COLOR(0x8595BD00));
static f32 bounce = 1.0f;
static f32 line_width = 0.08f;

static f32 vertexs[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f, 1.0f,

        1.0f, 1.0f,
        -1.0f, 1.0f,
        1.0f, -1.0f,
};

using namespace mfw;
struct Stick {
    static class Renderer {
    private:
        VertexArray m_vao;
        ShaderProgram m_shader;
        VertexBufferLayout cube_layout;
        VertexBuffer m_vbo;

    public:
        Renderer()
            : m_vbo(vertexs, sizeof(vertexs))
        {
            cube_layout.add<float>(2);
            m_vao.applyBufferLayout(cube_layout);

            m_shader.attachShader(GL_VERTEX_SHADER, "res/shaders/line.vert");
            m_shader.attachShader(GL_FRAGMENT_SHADER, "res/shaders/line.frag");
            m_shader.link();

            m_vao.unbind();
            m_vbo.unbind();
            m_shader.unbind();
        }

        inline void bind() {
            m_vao.bind();
            m_shader.bind();
        }

        inline void unbind() {
            m_shader.unbind();
            m_vao.unbind();
        }

        void render(const glm::mat4& o, const glm::vec2& p1, const glm::vec2& p2, glm::vec3 color, f32 w) {
            glm::mat4 view = glm::mat4(1);
            view = glm::translate(view, glm::vec3((p1 + p2) * 0.5f, 0));
            view = glm::rotate(view, glm::atan((p1.y - p2.y) / (p1.x - p2.x)), glm::vec3(0, 0, 1));
            view = glm::scale(view, glm::vec3(glm::length(p1 - p2) * 0.5, w, 0));
            m_shader.set3f("color", glm::value_ptr(color));
            m_shader.setMat4("view", o * view);
            GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
        }

    }* renderer;

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
        //glm::vec2 nd = glm::normalize(*p[0] - *p[1]) * (d - cd) * bounce / cd;
        glm::vec2 nd = glm::normalize(*p[0] - *p[1]) * (d - cd) * bounce * 0.5f;
        *p[0] += nd;
        *p[1] -= nd;
    }

    inline void render(const glm::mat4& o) {
        renderer->render(o, *p[0], *p[1], glm::vec3(1), line_width);
    }

    f32 d;
    glm::vec2* p[2];

};

Stick::Renderer* Stick::renderer = nullptr;

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
            entities.emplace_back(glm::vec2(0, i * d), glm::vec3((i + 1.0f) / node), r);
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

        entities.emplace_back(glm::vec2(lh, lh), node_color, r);
        entities.emplace_back(glm::vec2(lh, -lh), node_color, r);
        entities.emplace_back(glm::vec2(-lh, -lh), node_color, r);
        entities.emplace_back(glm::vec2(-lh, lh), node_color, r);

        sticks.emplace_back(&entities[0].m_pos, &entities[1].m_pos, d);
        sticks.emplace_back(&entities[1].m_pos, &entities[2].m_pos, d);
        sticks.emplace_back(&entities[2].m_pos, &entities[3].m_pos, d);
        sticks.emplace_back(&entities[3].m_pos, &entities[0].m_pos, d);
        sticks.emplace_back(&entities[0].m_pos, &entities[2].m_pos, d * std::sqrt(2.f));
        sticks.emplace_back(&entities[1].m_pos, &entities[3].m_pos, d * std::sqrt(2.f));
    }

    void init_triangle(f32 l) {
        this->node = 3;
        this->d = sqrt(3.f) * l;
        entities.reserve(3);
        sticks.reserve(3);
        entities.emplace_back(glm::vec2(0, l), node_color, r);
        entities.emplace_back(glm::vec2(d, -l) * 0.5f, node_color, r);
        entities.emplace_back(glm::vec2(-d, -l) * 0.5f, node_color, r);

        sticks.emplace_back(&entities[0].m_pos, &entities[1].m_pos, d);
        sticks.emplace_back(&entities[1].m_pos, &entities[2].m_pos, d);
        sticks.emplace_back(&entities[2].m_pos, &entities[0].m_pos, d);
    }

    void update() {
        for (auto& stick : sticks) {
            stick.update();
        }
    }

    void render(const glm::mat4& o) {
        Stick::renderer->bind();
        for (auto & stick : sticks) {
            stick.render(o);
        }
        Stick::renderer->unbind();
    }

    std::vector<Circle> entities;
    std::vector<Stick> sticks;
    i32 node, d;
};

class DemoSandBox : public Application {
private:
    glm::mat4 o;
    glm::vec2 ws;
    Circle::Renderer cm;

    f32 d = 2;
    std::vector<String> strings;
    Circle* holding = nullptr;

public:
    DemoSandBox()
    {
        Stick::renderer = new Stick::Renderer();
        Circle::renderer = new Circle::Renderer();

        auto window = GetWindow();
        ws = glm::vec2(window->width(), window->height()) / 60.f;
        o = glm::ortho(-ws.x, ws.x, -ws.y, ws.y, -1.0f, 1.0f);

        strings.push_back(String());
        strings.back().init_string(3, d);
        
        strings.push_back(String());
        strings.back().init_box(3);

        strings.push_back(String());
        strings.back().init_triangle(2);

        glClearColor(0.1, 0.1, 0.1, 0);
    }

    virtual void Update() override {
        f32 frame = 1.0 / 144;

        auto window = GetWindow();
        f32 width = window->width(), height = window->height();
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

        for (auto& string : strings) {
            string.update();
        }

        for (auto& string : strings) {
            string.render(o);
            Circle::renderer->bind();
            for (auto& e : string.entities) 
                cm.renderCircle(o, e);
            Circle::renderer->unbind();
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("config");

        //  #8595BD
        ImGui::SliderFloat("line width", &line_width, 0.02f, 0.3f);
        ImGui::SliderFloat("bounce", &bounce, 0.1f, 1.0f);
        if (ImGui::SliderFloat("node size", &r, 0.1f, 0.5f)) {
            for (auto& string : strings) {
                for (auto& e : string.entities) {
                    e.d = r;
                }
            }
        }
        if (ImGui::ColorEdit3("node color", glm::value_ptr(node_color))) {
            for (auto& string : strings) {
                for (auto& e : string.entities) {
                    e.m_color = node_color;
                }
            }
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

    virtual void OnMouseScroll(const MouseScrollEvent& event) override {
        (void)event;
    }

    virtual void OnWindowResize(const WindowResizeEvent& event) override {
        glViewport(0, 0, event.width, event.height);
        glm::vec2 ws = glm::vec2(event.width, event.height) / 60.f;
        o = glm::ortho(-ws.x, ws.x, -ws.y, ws.y, -1.0f, 1.0f);
    }

    ~DemoSandBox() {
        delete Stick::renderer;
        delete Circle::renderer;
    }

};

mfw::Application* mfw::CreateApplication() {
    return new DemoSandBox();
}

