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

using namespace mfw;
class SandBox : public Application {
private:
    void update() {
        main = GetWindow();
        width = main->width();
        height = main->height();
        auto& mouse = Input::GetMouse();
        this->mouse.x = mouse.first;
        this->mouse.y = mouse.second;
    }

public:
    i32 width, height;
    Window* main = nullptr;
    glm::vec2 mouse;

    SandBox() {
        Stick::renderer = new Stick::Renderer();
        Circle::renderer = new Circle::Renderer();
    }

    virtual void Start() override {
        update();
    };

    virtual void Update() override {
        update();
    }

    ~SandBox() {
        delete Stick::renderer;
        delete Circle::renderer;
    }

};

class Rigid2DSimlution : public SandBox {
private:
    struct Settings {
        i32 sub_step = 1;
        bool gravity = true;
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
    glm::vec2 world = glm::vec2(25, 10);
    f32 world_scale = 60;

    glm::vec2* holding = nullptr;
    Stick::Attribute attri = Stick::attribute;

    Circle* first_circle = nullptr;
    glm::vec2 first_pos;

    std::vector<Mesh*> meshes;
    std::vector<FixPoint*> fixPoints;

    Mode mode;

public:
    Rigid2DSimlution()
        : o(glm::mat4(1)), view(glm::mat4(1)), scale(glm::mat4(1)), mode(Mode::Normal)
    {}

    virtual void Start() override {
        SandBox::Start();

        glm::vec2 world = glm::vec2(width, height) / world_scale;
        o = glm::ortho(-world.x, world.x, -world.y, world.y, -1.0f, 1.0f);

        view = glm::translate(view, glm::vec3(0, 1, 0));

        InitMeshes();
        InitFixPoints();

        glClearColor(0.1, 0.1, 0.1, 0);
    }

    void InitMeshes() {
        meshes.push_back(new Mesh());

        //meshes.push_back(new Mesh());
        //InitString(*meshes.back(), 3, 3, glm::vec2(-1.5, 2));
        //meshes.push_back(new Mesh());
        //InitString(*meshes.back(), 5, 2, glm::vec2(1.5, 4));

        meshes.push_back(new Mesh());
        InitCircle(*meshes.back(), 2, 3, glm::vec2(0, 0));
        meshes.push_back(new Mesh());
        InitCircle(*meshes.back(), 2, 3, glm::vec2(0, 0));

        meshes.push_back(new Mesh());
        InitCircle(*meshes.back(), 2, 4, glm::vec2(0, 0));
        meshes.push_back(new Mesh());
        InitCircle(*meshes.back(), 2, 4, glm::vec2(0, 0));

        return;
        meshes.push_back(new Mesh());
        InitBox(*meshes.back(), 1, glm::vec2(-6, 2));
        meshes.push_back(new Mesh());
        InitBox(*meshes.back(), 3, glm::vec2(-6, 2));
        meshes.push_back(new Mesh());
        InitBox(*meshes.back(), 5, glm::vec2(-6, 2));

        meshes.push_back(new Mesh());
        InitTriangle(*meshes.back(), 1,  glm::vec2(6, 2));
        meshes.push_back(new Mesh());
        InitTriangle(*meshes.back(), 3,  glm::vec2(6, 2));
        meshes.push_back(new Mesh());
        InitTriangle(*meshes.back(), 5,  glm::vec2(6, 2));
    }

    void FreeMeshes() {
        for (auto& mesh : meshes){
            delete mesh;
        }
        meshes.clear();
    }

    void InitFixPoints() {
        for (i32 i = 0; i < 10; i++) {
            fixPoints.push_back(new FixPoint());
        }
    }

    void FreeFixPoints() {
        for (auto& fixPoint : fixPoints){
            delete fixPoint;
        }
        fixPoints.clear();
    }

    void update_physics(f32 dt) {
        for (auto& mesh : meshes) {
            for (auto& e : mesh->entities) {
                e->add_force(glm::vec2(0, -98.1 * e->m_mass * 1.8 * settings.gravity));
                e->update(dt);
            }
        }
    }

    void wall_collision(Circle* c, const glm::vec2& world) {
        if (c->m_pos.y - c->r < -world.y) {
            c->m_pos.y += -world.y - c->m_pos.y + c->r;
        }
        if (c->m_pos.x - c->r < -world.x) {
            c->m_pos.x += -world.x - c->m_pos.x + c->r;
        }
        else if (c->m_pos.x + c->r > world.x) {
            c->m_pos.x += world.x - c->m_pos.x - c->r;
        }
    };

    void update_collision() {
        if (holding) {
            glm::vec4 uv = glm::vec4(mouse.x / width, 1 - mouse.y / height, 0, 0) * 2.0f - 1.0f;
            glm::vec2 wpos = glm::vec2((view * (uv / o)) / scale);
            *holding = wpos;
        }
        for (auto& point : fixPoints)  {
            point->fix();
            //wall_collision(reinterpret_cast<Circle*>(&point), world);
        }
        for (auto& mesh : meshes) {
            for (auto& e : mesh->entities) {
                wall_collision(e, world);
            }
        }
    }

    void update_constraint() {
        for (auto& mesh : meshes) {
            mesh->update();
        }
    }

    virtual void Update() override {
        SandBox::Update();
        static f32 frame = 1.0 / 144;

        glClear(GL_COLOR_BUFFER_BIT);

        if (mode != Mode::Edit) {
            f32 dt = frame / settings.sub_step;
            for (i32 i = 0; i < settings.sub_step; i++) {
                update_physics(dt);
                update_collision();
                update_constraint();
            }
        }

        glm::mat4 proj = o * view * scale;

        for (auto& fixPoint : fixPoints) {
            fixPoint->render(proj);
        }

        for (auto& mesh : meshes) {
            mesh->render(proj);
        }

        for (auto& e : meshes[1]->entities) {
            Circle::renderer->draw(proj, *e);
        }

        Stick::renderer->draw(proj, glm::vec2(world.x, -world.y) - 0.2f, glm::vec2(-world.x, -world.y) - 0.2f, glm::vec3(1), 0.2);

        if (!Input::KeyPress(' ') && !Input::KeyPress(VK_CONTROL)) {
            mode = Mode::Normal;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("config");

        ImGui::Checkbox("gravity", &settings.gravity);
        ImGui::SliderInt("sub step", &settings.sub_step, 1, 20);

        bool motified = false;
        motified |= ImGui::SliderFloat("line width", &attri.line_width, 0.02f, 0.3f);
        motified |= ImGui::SliderFloat("bounce", &attri.bounce, 0.0f, 1.0f);
        motified |= ImGui::SliderFloat("node size", &attri.node_size, 0.1f, 0.5f);
        motified |= ImGui::ColorEdit3("node color", glm::value_ptr(attri.node_color));

        if (motified) {
            for (auto& mesh : meshes) {
                for (auto& stick : mesh->sticks) {
                    stick->attri = attri;
                }
                for (auto& e : mesh->entities) {
                    e->r = attri.node_size;
                }
            }
        }

        ImGui::Text("sticks: %d", GetSticksCount());

        if (ImGui::Button("reset")) {
            FreeMeshes();
            InitMeshes();
            FreeFixPoints();
            InitFixPoints();
            attri = Stick::attribute;
            settings.gravity = true;
            settings.sub_step = 1;
            mode = Mode::Normal;
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
                mode = Mode::Action;
            }
            return;
        }

    }

    virtual void OnCursorMove(const CursorMoveEvent& event) override {
        static i32 mx = event.x, my = event.y;
        if (mode == Mode::Action) {
            if (Input::MouseButtonDown(Left)) {
                view = glm::translate(view, glm::vec3(event.x - mx, my - event.y, 0) * 0.025f);
            }
            mx = event.x, my = event.y;
        }
    }

    virtual void OnMouseScroll(const MouseScrollEvent& event) override {
        if (mode == Mode::Action) {
            f32 val = 1 - event.ydelta * 0.04f;
            scale = glm::scale(scale, glm::vec3(val, val, 1));
        }
    }

    Circle* FindCircleByPosition(const glm::vec2& pos) {
        for (auto& mesh : meshes) {
            for (auto& e : mesh->entities) {
                if (e->r > glm::length(e->m_pos - pos)) {
                    return e;
                }
            }
        }
        return nullptr;
    }

    FixPoint* FindFixPointByPosition(const glm::vec2& pos) {
        for (auto& point : fixPoints) {
            if (point->r > glm::length(point->pos - pos)) {
                return point;
            }
        }
        return nullptr;
    }

    virtual void OnMouseButton(const MouseButtonEvent& event) override {
        glm::vec4 uv = glm::vec4(mouse.x / width, 1 - mouse.y / height, 0, 0) * 2.0f - 1.0f;
        glm::vec2 wpos = glm::vec2((view * (uv / o)) / scale);

        if (mode == Mode::Edit) {
            OnEdit(event, wpos);
        }
        else if (mode == Mode::Normal) {
            OnNormal(event, wpos);
        }

    }

    void OnEdit(const MouseButtonEvent& event, const glm::vec2& wpos) {
        if (event.button == MouseButton::Left && event.mode == KeyMode::Down) {
            first_circle = FindCircleByPosition(wpos);
            if (!first_circle) {
                first_pos = wpos;
            }
        } else if (event.button == MouseButton::Left && event.mode == KeyMode::Release) {
            Circle* second_circle = FindCircleByPosition(wpos);

            if (first_circle && second_circle) {
                f32 d = glm::length(first_circle->m_pos - second_circle->m_pos);
                meshes[0]->sticks.push_back(new Stick(&first_circle->m_pos, &second_circle->m_pos, d, attri));
            }
            else if (first_circle && second_circle == nullptr) {
                f32 d = glm::length(first_circle->m_pos - wpos);
                meshes[0]->entities.push_back(new Circle(wpos, glm::vec3(0, 0, 1), attri.node_size));
                meshes[0]->sticks.push_back(new Stick(&first_circle->m_pos, &meshes[0]->entities.back()->m_pos, d, attri));
            }
            else if (first_circle == nullptr && second_circle) {
                f32 d = glm::length(first_pos - second_circle->m_pos);
                meshes[0]->entities.push_back(new Circle(first_pos, glm::vec3(0, 0, 1), attri.node_size));
                meshes[0]->sticks.push_back(new Stick(&meshes[0]->entities.back()->m_pos, &second_circle->m_pos, d, attri));
            }
            else {
                auto first = new Circle(first_pos, glm::vec3(0, 0, 1), attri.node_size);
                meshes[0]->entities.push_back(first);
                auto second = new Circle(wpos, glm::vec3(0, 0, 1), attri.node_size);
                f32 d = glm::length(first->m_pos - second->m_pos);
                meshes[0]->entities.push_back(second);
                meshes[0]->sticks.push_back(new Stick(&first->m_pos, &second->m_pos, d, attri));
            }
        }
    }

    void OnNormal(const MouseButtonEvent& event, const glm::vec2& wpos) {
        if (event.button == MouseButton::Left && event.mode == KeyMode::Release) {
            if (holding) {
                FixPoint* fix = FindFixPointByPosition(*holding);
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
                Circle* circle = FindCircleByPosition(wpos);
                if (circle) {
                    holding = &circle->m_pos;
                }
            }
        } else {
            holding = nullptr;
        }

        // move fixpoint
        if (event.button == MouseButton::Right && event.mode == KeyMode::Down) {
            if (!holding) {
                FixPoint* fix = FindFixPointByPosition(wpos);
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
            Pattern<char>::Log('[', "");
            Pattern<f32>::Log(value.x, "");
            Pattern<const char*>::Log(", ", "");
            Pattern<f32>::Log(value.y, "");
            Pattern<char>::Log(']', "");
        }
    };

}

mfw::Application* mfw::CreateApplication() {
    return new Rigid2DSimlution();
}

