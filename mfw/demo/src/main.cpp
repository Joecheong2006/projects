#include <mfw.h>
#include "ScreenBufferTest.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"

using namespace mfw;
class DemoSandBox : public Application {
private:
    Test* test;
    bool fullScreen = false;

public:
    DemoSandBox(): Application("demo", 1280, 960)
    {
        test = new ScreenBufferTest();
    }

    virtual void Start() override {
        test->Start();
    }

    virtual void Update() override {
        test->Update();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin(test->name.c_str());
        test->UpdateImgui();
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    virtual void OnInputKey(const KeyEvent& event) override {
        if (event.key == VK_ESCAPE && event.mode == KeyMode::Down) {
            Terminate();
        }

        auto main = &Application::Get().GetWindow();

        if (Input::KeyPress('F')) {
            fullScreen = !fullScreen;
            main->setFullScreen(fullScreen);
            glViewport(0, 0, main->width(), main->height());
        }
    }

    virtual void OnWindowResize(const WindowResizeEvent& event) override {
    }

    virtual void OnWindowFocus(const WindowFocusEvent& event) override {
        LOG_EVENT_INFO(event);
        if (fullScreen) {
            GetWindow().setMode(WindowMode::Maximize);
        }
    }

    virtual void OnWindowNotFocus(const WindowNotFocusEvent& event) override {
        LOG_EVENT_INFO(event);
        if (fullScreen) {
            GetWindow().setMode(WindowMode::Minimize);
        }
    }

};

mfw::Application* mfw::CreateApplication() {
    return new DemoSandBox();
}
