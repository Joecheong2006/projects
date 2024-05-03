#include "PhysicsEmulator.h"

#include <mfw/mfw.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui/implot.h"

#include "DistanceConstraint.h"
#include "PointConstraint.h"
#include "Simulation.h"
#include "BuildObject.h"

namespace Log {
    template <>
    struct Pattern<vec2> {
        static void Log(const vec2& value, const std::string& format) {
            (void)format;
            LOG_INFO("[{}, {}]", value.x, value.y);
        }
    };
}

std::vector<Circle*> FindCirclesByPosition(const vec2& pos) {
    std::vector<Circle*> result;
    for (auto& obj : Simulation::Get()->world.getObjects<Circle>()) {
        auto circle = static_cast<Circle*>(obj);
        if (circle->radius > glm::length((const vec2&)circle->m_position - pos)) {
            result.push_back(circle);
        }
    }
    return result;
}

PointConstraint* FindPointConstraintByPosition(const vec2& pos) {
    auto& objects = Simulation::Get()->world.getConstraint<PointConstraint>();
    for (auto& obj : objects) {
        auto point = static_cast<PointConstraint*>(obj);
        if (point->d > glm::length((vec2)point->m_position - pos)) {
            return point;
        }
    }
    return nullptr;
}

Circle* FindCircleByPosition(const vec2& pos) {
    auto& objects = Simulation::Get()->world.getObjects<Circle>();
    for (auto& obj : objects) {
        auto point = static_cast<Circle*>(obj);
        if (point->radius > glm::length((const vec2&)point->m_position - pos)) {
            return point;
        }
    }
    return nullptr;
}

i32 GetObjectsCount() {
    auto simulation = Simulation::Get();
    auto& dc = simulation->world.getConstraint<DistanceConstraint>();
    auto& circles = simulation->world.getObjects<Circle>();
    return dc.size() + circles.size();
}

using namespace mfw;
void PhysicsEmulator::UpdateStatus() {
    width = GetWindow().width();
    height = GetWindow().height();
    auto& mouse = Input::GetMouse();
    this->mouse.x = mouse.first;
    this->mouse.y = mouse.second;
}

PhysicsEmulator::PhysicsEmulator()
    : Application("PE", 1440, 960), mode(Mode::Normal)
{}

PhysicsEmulator::~PhysicsEmulator() {
    ImPlot::DestroyContext();
}

void PhysicsEmulator::setSimulation(Simulation* simulation) {
    if (!simulation)
        return;
    if (Simulation::Instance)
        delete Simulation::Instance;
    Simulation::Instance = simulation;
    this->simulation = simulation;
}

void PhysicsEmulator::Start() {
    ImPlot::CreateContext();
    GetWindow().setVSync(true);
    UpdateStatus();

    if (simulation == nullptr) {
        LOG_FATAL("No vaild simulation instance\n");
        ImPlot::DestroyContext();
        ASSERT(true);
    }

    addUiLayer(new Info());

    simulation->OnStart();
    sub_dt = frame / simulation->world.getSubStep();

    real worldScale = simulation->getWorldUnit();
    world_scale *= worldScale;
    shift_rate *= worldScale;
    zoom_rate *= worldScale;

    preview.reserve(16);
    SetWorldProjection(vec2(width, height));

    glClearColor(COLOR(0x121414), 1);
}

#include <list>
static std::list<real> frameSamples(100);

void plotFramesDelay() {
    std::vector<real> samples;
    ImGui::BeginChild("ChildL", ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowContentRegionWidth() * 0.2), false, ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Frame[ms]:\n%f", frameSamples.front());
    ImGui::SameLine();
    ImPlot::StyleColorsDark();
    auto& style = ImPlot::GetStyle();
    style.PlotPadding = {};
    style.PlotMinSize = {};
    style.LineWeight = 3;
    if (ImPlot::BeginPlot("##Filter", { -1, -1 })) {
        ImPlot::SetupAxis(ImAxis_Y1, "", ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks);
        ImPlot::SetupAxis(ImAxis_X1, "", ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, 100);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 3000 / 144.0);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, 0, 100);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, 0, 3000.0 / 144);
        for (auto& frame : frameSamples) {
            samples.push_back(frame);
        }
        ImPlot::SetNextFillStyle({ COLOR(0x9ba8e2), 0.7 });
        ImPlot::PlotShaded("", samples.data(), samples.size());

        ImPlot::PushStyleColor(ImPlotCol_Line, { COLOR(0xb1bfff), 1 });
        ImPlot::PlotLine("", samples.data(), samples.size());
        ImPlot::EndPlot();
    }
    ImGui::EndChild();
}

void PhysicsEmulator::Update() {
    static real start = Time::GetCurrent();
    UpdateStatus();
    update(1.0 / fps); 
    render();
    frame = Time::GetCurrent() - start;
    frameSamples.push_back(frame * 1000);
    frameSamples.pop_front();
    start += frame;
    // printf("%g\n", frame * 1000.0);
}

void PhysicsEmulator::SetWorldProjection(vec2 view) {
    vec2 world = world_scale * vec2(view.x, view.y) / view.y;
    simulation->camera.ortho = glm::ortho(-world.x, world.x, -world.y, world.y, -1.0, 1.0);
}

void PhysicsEmulator::ApplySpringForce() {
    vec2 wpos = simulation->mouseToWorldCoord();
    real k = settings.mouseSpringForce;
    auto n = glm::normalize(wpos - rigidBodyHolder->m_position);
    real strengh = glm::length(wpos - rigidBodyHolder->m_position);
    rigidBodyHolder->addForce(glm::pow(strengh, 2) * n * k);
}

void PhysicsEmulator::MovePointConstraint() {
    vec2 wpos = simulation->mouseToWorldCoord();
    pointHolder->m_position = wpos;
}

void PhysicsEmulator::update(const real& dt) {
    (void)dt;
    Timer timer;
    if (!settings.pause && mode != Mode::Edit) {
        if (rigidBodyHolder) {
            ApplySpringForce();
        }
        if (pointHolder) {
            MovePointConstraint();
        }
        simulation->OnUpdate(dt);
        simulation->update(dt);
    }

    if ((i32)!Input::KeyPress(MF_KEY_SPACE) & (i32)!Input::KeyPress(MF_KEY_LEFT_CONTROL)) {
        mode = Mode::Normal;
    }
    update_frame = timer.getDuration();
}

#define DRAW_SPRING_SUB_STICK()\
    renderer.renderCircle(proj, Circle(p[1], w, color(0)));\
    renderer.renderCircle(proj, Circle(p[0], w, color(0)));\
    renderer.renderCircle(proj, Circle(p[1], w * 0.6, color(1)));\
    renderer.renderCircle(proj, Circle(p[0], w * 0.6, color(1)));\
    renderer.renderLine(proj, p[1], p[0], color(0), w);\
    renderer.renderLine(proj, p[1], p[0], color(1), w * 0.6);

void PhysicsEmulator::render() {
    Timer timer;
    renderer.clear();

    mat4 proj = simulation->camera.getProjection();

    if (settings.world_view) {
        simulation->render(renderer);
        simulation->OnRender(renderer);
    }

    const real worldScale = simulation->getWorldUnit();
    if (rigidBodyHolder) {
        const vec2 pos1 = simulation->mouseToWorldCoord();
        const vec2 pos2 = rigidBodyHolder->m_position;

        const real count = 12, len = 0.8 * worldScale, w = 0.07 * worldScale;
        const real n = glm::length(pos1 - pos2) / (count * worldScale);
        const vec2 normal = glm::normalize(pos1 - pos2) * worldScale;
        const vec2 t1 = glm::cross(vec3(normal, 0), vec3(0, 0, 1)) * len;
        const vec2 t2 = glm::cross(vec3(normal, 0), vec3(0, 0, -1)) * len;

        vec2 p[2];
        p[0] = t2 + pos2;
        p[1] = t1 + pos2;
        DRAW_SPRING_SUB_STICK();
        p[1] += normal * n;
        for (i32 i = 0; i < count; i++) {
            DRAW_SPRING_SUB_STICK();
            renderer.renderLine(proj, p[1], p[0], color(1), w * 0.5);
            p[i % 2] += normal * n * 2.0;
        }
        p[0] = t2 + pos1;
        p[1] = t1 + pos1;
        DRAW_SPRING_SUB_STICK();
    }

    if (settings.velocity_view || settings.acceleration_view) {
        auto& objects = simulation->world.getObjects<Circle>();
        for (auto& obj : objects) {
            RigidBody* body = static_cast<RigidBody*>(obj);
            if (settings.acceleration_view) {
                //vec2 a = body->m_acceleration * worldScale * 0.1;
                vec2 a = (body->m_velocity - body->m_ovelocity) * worldScale * 0.1 / sub_dt;
                renderer.renderLine(proj, body->m_position,
                        body->m_position + a * worldScale, blue, 0.02 * worldScale);
            }
            if (settings.velocity_view) {
                vec2 v = body->m_velocity * worldScale;
                renderer.renderLine(proj, body->m_position, 
                        body->m_position + v * worldScale, red, 0.02 * worldScale);
            }
        }
    }

    render_frame = timer.getDuration();
}

void PhysicsEmulator::renderImgui() {
    ImGui::BeginTabBar("tabBar");
    static real startTime = 0;
    if (ImGui::BeginTabItem("config")) {
        ImGui::Text("objects:%d", GetObjectsCount());
        ImGui::Text("unit:%gcm", simulation->getWorldUnit() * 100);
        ImGui::Text("Step:%d", simulation->world.getSubStep());

        ImGui::Checkbox("pause", &settings.pause);
        float gravity[2] = {
            (float)simulation->world.gravity.x,
            (float)simulation->world.gravity.y,
        };
        ImGui::SliderFloat2("gravity", gravity, -10, 10);
        simulation->world.gravity = vec2(gravity[0], gravity[1]);

        ImGui::Checkbox("world view", &settings.world_view);
        ImGui::Checkbox("velocity view", &settings.velocity_view);
        ImGui::Checkbox("acceleration view", &settings.acceleration_view);
        float mouseSpringForce = settings.mouseSpringForce;
        ImGui::SliderFloat("mouse fource", &mouseSpringForce, 1, 100);
        settings.mouseSpringForce = mouseSpringForce;

        if (ImGui::Button("restart")) {
            restart();
            startTime = Time::GetCurrent();
        }
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("status")) {
        ImGui::Text("SF:%-8d", (i32)(1.0 / sub_dt));
        ImGui::SameLine();
        ImGui::Text("ST[ms]:%-8.2g", 1000.0 * update_frame);
        ImGui::SameLine();
        ImGui::Text("RT[ms]:%-8.2g", 1000.0 * render_frame);
        if (settings.pause) {
            startTime = Time::GetCurrent();
            ImGui::Text("Time:%g", 0.0);
        }
        else {
            ImGui::Text("Time:%g", Time::GetCurrent() - startTime);
        }

        {
            static real fps = 0, averCount = 0, averFps = 1.0 / frame, averTake = int(this->fps / 15);
            fps += 1.0f / frame;
            if (++averCount >= averTake) {
                averFps = fps / averTake;
                fps = 0;
                averCount = 0;
            }
            ImGui::Text("FPS:%5.2f", averFps);
        }

        plotFramesDelay();
        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();

    if (mode != Mode::Action) {
        return;
        auto objects = FindCirclesByPosition(simulation->mouseToWorldCoord());
        for (auto& prev : preview) {
            prev->m_color = Circle::default_color;
        }
        for (auto& circle : objects) {
            preview.emplace_back(circle);
        }
        for (auto& circle : objects) {
            circle->m_color = Circle::default_color * 0.7f;
        }
    }
}

void PhysicsEmulator::restart() {
    simulation->world.clear();
    simulation->OnStart();
    preview.clear();
    mode = Mode::Normal;
}

bool PhysicsEmulator::OnInputKey(const KeyEvent& event) {
    if (event.key == MF_KEY_ESCAPE && event.mode == KeyMode::Down) {
        Terminate();
    }

    if (event.key == MF_KEY_R && event.mode == KeyMode::Down) {
        restart();
    }

    if (event.key == MF_KEY_P && event.mode == KeyMode::Down) {
        settings.pause = true;
    }
    else if (event.key == MF_KEY_P && event.mode == KeyMode::Release) {
        settings.pause = false;
    }
    
    if (mode == Mode::Normal) {
        if (event.key == MF_KEY_LEFT_CONTROL && event.mode == Down && !rigidBodyHolder) {
            mode = Mode::Edit;
        }
        if (event.key == MF_KEY_SPACE && event.mode == Down && !rigidBodyHolder) {
            for (auto& prev : preview) {
                prev->m_color = Circle::default_color;
            }
            mode = Mode::Action;
        }
    }

    static bool fullScreen = false;
    if (event.key == MF_KEY_F && event.mode == KeyMode::Down) {
        fullScreen = !fullScreen;
        GetWindow().setFullScreen(fullScreen);
    }
    return true;
}

bool PhysicsEmulator::OnCursorMove(const CursorMoveEvent& event) {
    static vec2 m = vec2(event.x, event.y);
    if (mode == Mode::Action && Input::MouseButtonDown(MF_MOUSE_BUTTON_LEFT)) {
        simulation->camera.view = glm::translate(simulation->camera.view, vec3(event.x - m.x, m.y - event.y, 0) * shift_rate);
    }
    m = vec2(event.x, event.y);
    return true;
}

bool PhysicsEmulator::OnMouseScroll(const MouseScrollEvent& event) {
    auto& simulation = *Simulation::Get();
    if (mode == Mode::Action) {
        real val = 1 + event.ydelta * zoom_rate;
        simulation.camera.scale = glm::scale(simulation.camera.scale, vec3(val, val, 1.0));
    }
    return true;
}

bool PhysicsEmulator::OnMouseButton(const MouseButtonEvent& event) {
    auto& simulation = *Simulation::Get();
    const vec2 wpos = simulation.mouseToWorldCoord();

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
    return true;
}

void PhysicsEmulator::OnEdit(const MouseButtonEvent& event, const vec2& wpos) {
    static Circle* preview_node = nullptr;
    static PointConstraint* preview_fix_point = nullptr;
    static vec2 preview_pos;
    const real worldScale = simulation->getWorldUnit();
    const real len_count_scale = 4.0 / worldScale;
    auto& world = simulation->world;
    if (event.button == MF_MOUSE_BUTTON_RIGHT && event.mode == KeyMode::Down) {
        preview_node = FindCircleByPosition(wpos);
        preview_fix_point = FindPointConstraintByPosition(wpos);
        if (!preview_node) {
            preview_pos = wpos;
        }
    } else if (event.button == MF_MOUSE_BUTTON_RIGHT && event.mode == KeyMode::Release) {
        Circle* second_node = FindCircleByPosition(wpos);
        PointConstraint* point = FindPointConstraintByPosition(wpos);

        if (preview_node && second_node) {
            if (preview_node == second_node) {
                return;
            }
            real d = glm::length(preview_node->m_position - second_node->m_position) / worldScale;
            auto dc = BuildObject<Spring>(preview_node, second_node, d);
            dc->count = dc->d * len_count_scale;
        }
        else if (preview_node && second_node == nullptr) {
            real d = glm::length(preview_node->m_position - wpos) / worldScale;
            auto p = world.addRigidBody<Circle>(wpos);
            auto dc = BuildObject<Spring>(preview_node, p, d);

            if (point) {
                point->target = p;
                dc->d = glm::length(point->m_position - preview_node->m_position) / worldScale;
            }
            dc->count = dc->d * len_count_scale;
        }
        else if (preview_node == nullptr && second_node) {
            real d = glm::length(preview_pos - second_node->m_position) / worldScale;
            auto p = world.addRigidBody<Circle>(preview_pos);
            auto dc = BuildObject<Spring>(p, second_node, d);

            if (preview_fix_point) {
                preview_fix_point->target = p;
                dc->d = glm::length(preview_fix_point->m_position - second_node->m_position) / worldScale;
            }
            dc->count = dc->d * len_count_scale;
        }
        else {
            if (preview_pos == wpos) {
                world.addRigidBody<Circle>(preview_pos);
                return;
            }
            real d = glm::length(preview_pos - wpos) / worldScale;
            if (d < Circle::default_radius * 2.0) {
                return;
            }

            auto p1 = world.addRigidBody<Circle>(preview_pos);
            auto p2 = world.addRigidBody<Circle>(wpos);
            auto dc = BuildObject<Spring>(p1, p2, d);

            if (point && preview_fix_point) {
                point->target = static_cast<RigidBody*>(p2);
                preview_fix_point->target = static_cast<RigidBody*>(p1);
                dc->d = glm::length(point->m_position - preview_fix_point->m_position);
            }
            else if (preview_fix_point) {
                preview_fix_point->target = static_cast<RigidBody*>(p1);
                dc->d = glm::length(preview_fix_point->m_position - wpos);
            }
            else if (point) {
                point->target = static_cast<RigidBody*>(p2);
                dc->d = glm::length(p2->m_position - p1->m_position);
            }
            dc->count = dc->d * len_count_scale;
        }
    }

    if (event.button == MF_MOUSE_BUTTON_LEFT && event.mode == KeyMode::Down) {
        if (!rigidBodyHolder) {
            rigidBodyHolder = FindCircleByPosition(wpos);
            if (rigidBodyHolder) {
                catch_offset = wpos - rigidBodyHolder->m_position;
            }
        }
    }
    else {
        rigidBodyHolder = nullptr;
        pointHolder = nullptr;
    }

    if (event.button == MF_MOUSE_BUTTON_LEFT && event.mode == KeyMode::Down) {
        if (!rigidBodyHolder) {
            auto pc = FindPointConstraintByPosition(wpos);
            if (pc) {
                pointHolder = pc;
            }
        }
    }
}

void PhysicsEmulator::OnNormal(const MouseButtonEvent& event, const vec2& wpos) {
    if (event.button == MF_MOUSE_BUTTON_LEFT && event.mode == KeyMode::Release) {
        if (rigidBodyHolder) {
            PointConstraint* point = FindPointConstraintByPosition(rigidBodyHolder->m_position);
            if (point) {
                point->target = static_cast<RigidBody*>(rigidBodyHolder);
                rigidBodyHolder = nullptr;
            }
        }
    }

    if (event.button == MF_MOUSE_BUTTON_LEFT && event.mode == KeyMode::Down) {
        if (!rigidBodyHolder) {
            PointConstraint* point = FindPointConstraintByPosition(wpos);
            rigidBodyHolder = FindCircleByPosition(wpos);
            if (rigidBodyHolder) {
                catch_offset = wpos - (vec2)rigidBodyHolder->m_position;
            }
            if (rigidBodyHolder && point) {
                rigidBodyHolder = point->target;
                point->target = nullptr;
            }
        }
    }
    else {
        rigidBodyHolder = nullptr;
        pointHolder = nullptr;
    }

    if (event.button == MF_MOUSE_BUTTON_RIGHT && event.mode == KeyMode::Down) {
        if (!rigidBodyHolder) {
            auto pc = FindPointConstraintByPosition(wpos);
            if (pc) {
                pointHolder = pc;
            }
        }
    }
}

bool PhysicsEmulator::OnWindowResize(const WindowResizeEvent& event) {
    glViewport(0, 0, event.width, event.height);
    SetWorldProjection(vec2(event.width, event.height));
    return true;
}

bool PhysicsEmulator::OnWindowFocus(const WindowFocusEvent& event) {
    (void)event;
    mode = Mode::Normal;
    return true;
}

bool PhysicsEmulator::OnWindowNotFocus(const mfw::WindowNotFocusEvent& event) {
    (void)event;
    mode = Mode::Normal;
    return true;
}

bool PhysicsEmulator::Info::OnUiRender() {
    PhysicsEmulator* emulator = static_cast<PhysicsEmulator*>(&mfw::Application::Get());
    if (!emulator)
        return false;
    emulator->renderImgui();
    return ImGui::IsWindowFocused();
}

