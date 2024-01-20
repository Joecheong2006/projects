#include <mfw.h>

class DemoSandBox : public mfw::Application {
public:
    DemoSandBox() {
    }

    virtual void Update() override {
    }

    ~DemoSandBox() {
    }

};

mfw::Application* mfw::CreateApplication() {
    return new DemoSandBox();
}
