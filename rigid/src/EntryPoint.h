#pragma once

#if defined(_WIN32) || defined(WIN32)

extern mfw::Application* mfw::CreateApplication();

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    mfw::Application::Get()->run();
    delete mfw::Application::Get();
    return 0;
}

#endif

