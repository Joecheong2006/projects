#pragma once
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture2D.h"
#include "stb_image.h"
#include "glfw3.h"
#include "imgui/imgui.h"

class Application {
private:
    GLFWwindow* m_window;
    ImGuiIO m_imguiIO;
    static ShaderProgram* currentShader;
    static std::string shaderName;
    static void loadShader(const std::string& name);
    friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

public:
    Application();
    ~Application();

    void run();

};
