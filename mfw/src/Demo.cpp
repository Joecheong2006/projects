#include <mfw.h>

class DemoSandBox : public mfw::Application {
public:
    DemoSandBox() {
    }

    ~DemoSandBox() {
    }

};

mfw::Application* mfw::CreateApplication() {
    return  new DemoSandBox();
}
