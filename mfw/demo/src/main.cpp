#include <mfw.h>
#include "KeyCode.h"
#include "ScreenBufferTest.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"

using namespace mfw;
class DemoSandBox : public Application {
private:
    bool fullScreen = false;

public:
    DemoSandBox(): Application("demo", 1280, 720)
    {}

    virtual void Start() override {
        addLayer(new ScreenBufferTest());
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

class DebugLayer : public Layer {
public:
    DebugLayer(const std::string& name): Layer(name) {}
    ~DebugLayer() { LOG_INFO("{}\n", (char*)__func__); }
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

class ImguiLayer : public Layer {
public:
    ImguiLayer(std::string name): Layer(name) {}
    virtual void OnUpdateImgui() = 0;
    bool focus = false;

private:
    inline virtual bool OnMouseButton(const MouseButtonEvent& event) override { TOVOID(event); return focus; }
    inline virtual bool OnMouseScroll(const MouseScrollEvent& event) override { TOVOID(event); return focus; }
    inline virtual bool OnWindowResize(const WindowResizeEvent& event) override { TOVOID(event); return focus; }
    inline virtual bool OnWindowClose(const WindowCloseEvent& event) override { TOVOID(event); return focus; }
    inline virtual bool OnCursorMove(const CursorMoveEvent& event) override { TOVOID(event); return focus; }
    inline virtual bool OnWindowNotFocus(const WindowNotFocusEvent& event) override { TOVOID(event); return focus; }
    inline virtual bool OnInputKey(const KeyEvent& event) override { TOVOID(event); return focus; }
};

class TestImguiLayer : public UiLayer {
public:
    TestImguiLayer(std::string name = "TestImguiLayer"): UiLayer(name) {}
    virtual bool OnUiRender() override {
        ImGui::Text("hi from %s", name.c_str());
        return ImGui::IsWindowFocused();
    }

};

class App : public Application {
public:
    App(): Application("demo", 960, 640)
    {}

    virtual void Start() override {
        glViewport(0, 0, GetWindow().width(), GetWindow().height());
        addLayer(new DebugLayer("testing"));
        addUiLayer(new TestImguiLayer());
        addUiLayer(new TestImguiLayer());
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

