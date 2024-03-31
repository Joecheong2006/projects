#include "PhysicsEmulator.h"

#include <mfw.h>

#include "EventSystem.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui/implot.h"

#include "DistanceConstraint.h"
#include "Circle.h"

namespace Log {
    template <>
    struct Pattern<glm::vec2> {
        static void Log(const glm::vec2& value, const std::string& format) {
            (void)format;
            LOG_INFO("[{}, {}]", value.x, value.y);
        }
    };
}

Simulation* PhysicsEmulator::sim = nullptr;

std::vector<Circle*> FindCirclesByPosition(const glm::vec2& pos) {
    std::vector<Circle*> result;
    for (auto& obj : PhysicsEmulator::sim->world.getObjects<Circle>()) {
        auto circle = static_cast<Circle*>(obj);
        if (circle->r > glm::length((const glm::vec2&)circle->m_pos - pos)) {
            result.push_back(circle);
        }
    }
    return result;
}

PointConstraint* FindPointConstraintByPosition(const glm::vec2& pos) {
    auto& objects = PhysicsEmulator::sim->world.getConstraint<PointConstraint>();
    for (auto& obj : objects) {
        auto point = static_cast<PointConstraint*>(obj);
        if (point->d > glm::length((glm::vec2)point->self.m_pos - pos)) {
            return point;
        }
    }
    return nullptr;
}

Circle* FindCircleByPosition(const glm::vec2& pos) {
    auto& objects = PhysicsEmulator::sim->world.getObjects<Circle>();
    for (auto& obj : objects) {
        auto point = static_cast<Circle*>(obj);
        if (point->r > glm::length((const glm::vec2&)point->m_pos - pos)) {
            return point;
        }
    }
    return nullptr;
}

i32 GetObjectsCount() {
    auto& dc = PhysicsEmulator::sim->world.getConstraint<DistanceConstraint>();
    auto& circles = PhysicsEmulator::sim->world.getObjects<Circle>();
    return dc.size() + circles.size();
}

using namespace mfw;
void PhysicsEmulator::UpdateStatus() {
    mfw::Window* main = GetWindow();
    width = main->width();
    height = main->height();
    auto& mouse = Input::GetMouse();
    this->mouse.x = mouse.first;
    this->mouse.y = mouse.second;
}

PhysicsEmulator::PhysicsEmulator(Simulation* simluation)
    : Application(simluation->name, 1440, 960), mode(Mode::Normal)
{
    sim = simluation;
    ASSERT(sim != nullptr);

    // world_scale = 9 * simu->unitScale;
    // shift_rate = 0.001 * world_scale;
    // zoom_rate = 0.01 * world_scale;
}

PhysicsEmulator::~PhysicsEmulator() {
    if (sim) {
        delete sim;
    }
    ImPlot::DestroyContext();
}

void PhysicsEmulator::Start() {
    ImPlot::CreateContext();
    GetWindow()->setVSync(true);
    UpdateStatus();

    sim->initialize();

    preview.reserve(16);
    SetWorldProjection(glm::vec2(width, height));

    glClearColor(COLOR(0x121414), 1);
}

#include <list>
static std::list<f32> frameSamples(100);

void plotFramesDelay() {
    std::vector<f32> samples;
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
    static f64 start = Time::GetCurrent();
    UpdateStatus();
    update(1.0 / fps); 
    render();
    renderImgui();
    frame = Time::GetCurrent() - start;
    frameSamples.push_back(frame * 1000);
    frameSamples.pop_front();
    start += frame;
    // printf("%g\n", frame * 1000.0);
}

void PhysicsEmulator::SetWorldProjection(glm::vec2 view) {
    glm::vec2 world = world_scale * glm::vec2(view.x, view.y) / view.y;
    sim->camera.ortho = glm::ortho(-world.x, world.x, -world.y, world.y, -1.0f, 1.0f);
}

void PhysicsEmulator::ApplyUserInputToScene() {
    glm::dvec2 wpos = sim->mouseToWorldCoord();
    if (holding->getTypeId() < 0) {
        holding->m_pos = wpos;
        return;
    }
    f64 k = (f64)settings.sub_step * 20;
    auto n = glm::normalize(wpos - holding->m_pos);
    f64 strengh = glm::length(wpos - holding->m_pos);
    holding->addForce(glm::pow(strengh, 2) * n * k);
}

void PhysicsEmulator::update(const f64& dt) {
    Timer timer;
    if (!settings.pause && mode != Mode::Edit) {
        sub_dt = dt / (f64)settings.sub_step;
        for (i32 i = 0; i < settings.sub_step; ++i) {
            sim->update(sub_dt);
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

void PhysicsEmulator::render() {
    Timer timer;
    renderer.clear();

    glm::mat4 proj = sim->camera.getProjection();

    if (settings.world_view) {
        sim->render(renderer);
    }

    auto& unitScale = sim->unitScale;
    if (settings.velocity_view || settings.acceleration_view) {
        auto& objects = sim->world.getObjects<Circle>();
        for (auto& obj : objects) {
            if (settings.acceleration_view) {
                glm::dvec2 a = (obj->m_velocity - obj->m_ovelocity) / (sub_dt * 20.0);
                a = obj->m_acceleration / 20.0;
                renderer.renderLine(proj, obj->m_pos,
                        obj->m_pos + a * (f64)unitScale, blue, 0.02 * unitScale);
            }
            if (settings.velocity_view) {
                glm::dvec2 v = obj->m_velocity / 6.0;
                renderer.renderLine(proj, obj->m_pos, 
                        obj->m_pos + v * (f64)unitScale, red, 0.02 * unitScale);
            }
        }
    }

    if (holding) {
        const glm::dvec2 m = sim->mouseToWorldCoord();
        renderer.renderLine(proj, m, holding->m_pos, glm::vec3(COLOR(0xb92c2c)), 0.1 * unitScale);
    }

    render_frame = timer.getDuration();
}

void PhysicsEmulator::renderImgui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("info");

    ImGui::BeginTabBar("tabBar");
    if (ImGui::BeginTabItem("config")) {
        auto& unitScale = sim->unitScale;
        ImGui::Text("objects:%d", GetObjectsCount());
        ImGui::Text("unit:%gcm", unitScale * 100);
        ImGui::Text("Step:%d", settings.sub_step);

        ImGui::Checkbox("pause", &settings.pause);
        ImGui::Checkbox("gravity", &settings.gravity);
        ImGui::Checkbox("world view", &settings.world_view);
        ImGui::Checkbox("velocity view", &settings.velocity_view);
        ImGui::Checkbox("acceleration view", &settings.acceleration_view);

        if (ImGui::Button("restart")) {
            restart();
        }
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("status")) {
        ImGui::Text("SF:%-6d", (i32)(1.0 / sub_dt));
        ImGui::SameLine();
        ImGui::Text("ST[ms]:%-6.2g", 1000.0 * update_frame);
        ImGui::SameLine();
        ImGui::Text("RT[ms]:%-6.2g", 1000.0 * render_frame);

        {
            static f32 fps = 0, averCount = 0, averFps = 1.0 / frame, averTake = int(this->fps / 15);
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

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    auto& attri = sim->attri;
    if (mode != Mode::Action) {
        auto objects = FindCirclesByPosition(sim->mouseToWorldCoord());
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

void PhysicsEmulator::restart() {
    sim->world.clear();
    sim->initialize();
    preview.clear();
    mode = Mode::Normal;
}

void PhysicsEmulator::OnInputKey(const KeyEvent& event) {
    if (event.key == VK_ESCAPE && event.mode == KeyMode::Down) {
        Terminate();
    }

    if (event.key == 'R' && event.mode == KeyMode::Down) {
        restart();
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
                prev->m_color = sim->attri.node_color;
            }
            mode = Mode::Action;
        }
    }

    auto* main = GetWindow();

    static bool fullScreen = false;
    if (event.key == 'F' && event.mode == KeyMode::Down) {
        fullScreen = !fullScreen;
        main->setFullScreen(fullScreen);
    }
}

void PhysicsEmulator::OnCursorMove(const CursorMoveEvent& event) {
    static glm::vec2 m = glm::vec2(event.x, event.y);
    if (mode == Mode::Action && Input::MouseButtonDown(Left)) {
        sim->camera.view = glm::translate(sim->camera.view, glm::vec3(event.x - m.x, m.y - event.y, 0) * shift_rate);
    }
    m = glm::vec2(event.x, event.y);
}

void PhysicsEmulator::OnMouseScroll(const MouseScrollEvent& event) {
    if (mode == Mode::Action) {
        f32 val = 1 + event.ydelta * zoom_rate;
        sim->camera.scale = glm::scale(sim->camera.scale, glm::vec3(val, val, 1));
    }
}

void PhysicsEmulator::OnMouseButton(const MouseButtonEvent& event) {
    const glm::dvec2 wpos = sim->mouseToWorldCoord();

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

void PhysicsEmulator::OnEdit(const MouseButtonEvent& event, const glm::dvec2& wpos) {
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
        const auto& attri = sim->attri;

        if (preview_node && second_node) {
            if (preview_node == second_node) {
                return;
            }
            f64 d = glm::length(preview_node->m_pos - second_node->m_pos);
            sim->world.addConstraint<DistanceConstraint>(preview_node, second_node, d, attri.line_width);
        }
        else if (preview_node && second_node == nullptr) {
            f64 d = glm::length(preview_node->m_pos - wpos);
            auto p = sim->world.addObject<Circle>(wpos, attri.node_color, attri.node_size);
            auto dc = sim->world.addConstraint<DistanceConstraint>(preview_node, p, d, attri.line_width);

            p->m_color = attri.node_color;
            p->r = attri.node_size;

            if (point) {
                point->target = p;
                dc->d = glm::length(point->self.m_pos - preview_node->m_pos);
            }
        }
        else if (preview_node == nullptr && second_node) {
            f64 d = glm::length(preview_pos - second_node->m_pos);
            auto p = sim->world.addObject<Circle>(preview_pos, attri.node_color, attri.node_size);
            auto dc = sim->world.addConstraint<DistanceConstraint>(p, second_node, d, attri.line_width);

            p->m_color = attri.node_color;
            p->r = attri.node_size;

            if (preview_fix_point) {
                preview_fix_point->target = p;
                dc->d = glm::length(preview_fix_point->self.m_pos - second_node->m_pos);
            }
        }
        else {
            if (preview_pos == wpos) {
                sim->world.addObject<Circle>(preview_pos, attri.node_color, attri.node_size);
                return;
            }
            f32 d = glm::length(preview_pos - wpos);
            if (d < attri.node_size * 2) {
                return;
            }

            auto p1 = sim->world.addObject<Circle>(preview_pos, attri.node_color, attri.node_size);
            auto p2 = sim->world.addObject<Circle>(wpos, attri.node_color, attri.node_size);
            auto dc = sim->world.addConstraint<DistanceConstraint>(p1, p2, d, attri.line_width);

            p1->m_color = attri.node_color;
            p1->r = attri.node_size;
            p2->m_color = attri.node_color;
            p2->r = attri.node_size;

            if (point && preview_fix_point) {
                point->target = static_cast<Object*>(p2);
                preview_fix_point->target = static_cast<Object*>(p1);
                dc->d = glm::length(point->self.m_pos - preview_fix_point->self.m_pos);
            }
            else if (preview_fix_point) {
                preview_fix_point->target = static_cast<Object*>(p1);
                dc->d = glm::length(preview_fix_point->self.m_pos - wpos);
            }
            else if (point) {
                point->target = static_cast<Object*>(p2);
                dc->d = (f32)glm::length(point->self.m_pos - preview_node->m_pos);
            }
        }

    }
}

void PhysicsEmulator::OnNormal(const MouseButtonEvent& event, const glm::vec2& wpos) {
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
            holding = FindCircleByPosition(wpos);
            if (holding) {
                catch_offset = wpos - (glm::vec2)holding->m_pos;
            }
        }
    }
    else {
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

void PhysicsEmulator::OnWindowResize(const WindowResizeEvent& event) {
    glViewport(0, 0, event.width, event.height);
    SetWorldProjection(glm::vec2(event.width, event.height));
}

void PhysicsEmulator::OnWindowFocus(const WindowFocusEvent& event) {
    (void)event;
    mode = Mode::Normal;
}

void PhysicsEmulator::OnWindowNotFocus(const mfw::WindowNotFocusEvent& event) {
    (void)event;
    mode = Mode::Normal;
}

