#include <mfw.h>
#include "KeyCode.h"
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
    DemoSandBox(): Application("demo", 1280, 720)
    {
        test = new ScreenBufferTest();
    }

    ~DemoSandBox() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    virtual void Start() override {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.ConfigFlags |= ImGuiBackendFlags_HasSetMousePos;

        ImGui::StyleColorsDark();
        ImGui_ImplWin32_InitForOpenGL(Application::Get().GetWindow().getHandle());
        ImGui_ImplOpenGL3_Init("#version 410");
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

    virtual bool OnInputKey(const KeyEvent& event) override {
        if (event.key == MF_KEY_ESCAPE && event.mode == KeyMode::Down) {
            Terminate();
        }

        auto main = &Application::Get().GetWindow();

        if (Input::KeyPress(MF_KEY_F)) {
            fullScreen = !fullScreen;
            main->setFullScreen(fullScreen);
            glViewport(0, 0, main->width(), main->height());
        }
        return true;
    }

    virtual bool OnWindowFocus(const WindowFocusEvent& event) override {
        LOG_EVENT_INFO(event);
        if (fullScreen) {
            GetWindow().setMode(WindowMode::Maximize);
        }
        return true;
    }

    virtual bool OnWindowNotFocus(const WindowNotFocusEvent& event) override {
        LOG_EVENT_INFO(event);
        if (fullScreen) {
            GetWindow().setMode(WindowMode::Minimize);
        }
        return true;
    }

};

class TestLayer : public Layer {
public:
    TestLayer(const std::string& name): Layer(name) {}
    ~TestLayer() { LOG_INFO("{}\n", (char*)__func__); }
    virtual void OnStart() override { LOG_INFO("{}\n", (char*)__func__); }
    virtual bool OnMouseButton(const MouseButtonEvent& event) override { LOG_EVENT_INFO(event); return true; }
    virtual bool OnMouseScroll(const MouseScrollEvent& event) override { LOG_EVENT_INFO(event); return true; }
    virtual bool OnWindowResize(const WindowResizeEvent& event) override { LOG_EVENT_INFO(event); return true; }
    virtual bool OnWindowClose(const WindowCloseEvent& event) override { LOG_EVENT_INFO(event); return true; }
    virtual bool OnCursorMove(const CursorMoveEvent& event) override { LOG_EVENT_INFO(event); return true; }
    virtual bool OnWindowFocus(const WindowFocusEvent& event) override { LOG_EVENT_INFO(event); return true; }
    virtual bool OnWindowNotFocus(const WindowNotFocusEvent& event) override { LOG_EVENT_INFO(event); return true; }

    virtual bool OnInputKey(const KeyEvent& event) override {
        LOG_INFO("{}\n", (char)event.key);
        if (event.key == MF_KEY_TAB) {
        LOG_INFO("TAB");
        }
        return false;
    }

};

class TestImgui : Layer {
public:
    TestImgui(): Layer("testImgui") {}
    virtual void OnUpdate() override {
        ImGui::Text("hi");
    }
};

class App : public Application {
public:
    App(): Application("demo", 960, 640)
    {
    }

    virtual void Start() override {
        //addLayer(imguiLayer);
        glViewport(0, 0, GetWindow().width(), GetWindow().height());
    }

    virtual void Update() override {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    virtual bool OnInputKey(const KeyEvent& event) override {
        if (event.key == MF_KEY_ESCAPE)
            Terminate();
        if (event.key == MF_KEY_LEFT_CONTROL) {
            LOG_INFO("lshift");
        }
        return false;
    }

};

mfw::Application* mfw::CreateApplication() {
    // return new App();
    return new DemoSandBox();
}
