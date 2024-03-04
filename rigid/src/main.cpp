#include <mfw.h>
#include "Circle.h"
#include "glm/gtc/matrix_transform.hpp"

static f32 vertex[] = {
     1,  1,
    -1,  1,
     1, -1,
    -1, -1,
};

static u32 index[] = {
    1, 0, 2,
    1, 3, 2,
};

using namespace mfw;
class DemoSandBox : public Application {
private:
    glm::mat4 o;
    Circle::Manager cm;

public:
    DemoSandBox()
    {
        cm.createCircle(glm::vec2(0), glm::vec3(1), 10);
        auto window = GetWindow();
        glm::vec2 ws = glm::vec2(window->width(), window->height()) / 20.f;
        o = glm::ortho(-ws.x, ws.x, -ws.y, ws.y, -1.0f, 1.0f);

        glClearColor(0, 0, 0, 0);
    }

    virtual void Update() override {
        f32 frame = 1.0 / 144;

        auto window = GetWindow();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        cm.renderCircle(o);

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

    virtual void OnWindowResize(const WindowResizeEvent& event) override {
        glViewport(0, 0, event.width, event.height);
        glm::vec2 ws = glm::vec2(event.width, event.height) / 20.f;
        o = glm::ortho(-ws.x, ws.x, -ws.y, ws.y, -1.0f, 1.0f);
    }

    ~DemoSandBox() {
    }

};

mfw::Application* mfw::CreateApplication() {
    return new DemoSandBox();
}
