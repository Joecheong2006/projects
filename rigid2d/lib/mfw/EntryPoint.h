#pragma once

#if defined(_WIN32) || defined(WIN32)

extern mfw::Application* mfw::CreateApplication();

int main() {
    mfw::Application* app = mfw::CreateApplication();
    app->run();
    delete app;
}

#endif

