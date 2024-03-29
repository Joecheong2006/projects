#pragma once

#if defined(_WIN32) || defined(WIN32)

extern mfw::Application* mfw::CreateApplication();

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    mfw::Application* app = mfw::CreateApplication();
    app->run();
    delete app;
    return 0;
}

#endif

