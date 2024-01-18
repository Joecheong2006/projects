#include "Application.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <fstream>
#include <sys/stat.h>
#include <cstring>

#define SHADER_SOURCE_DIRECTORY "shaders/"
#define DEFAULT_SHADER_NAME "default"

ShaderProgram* Application::currentShader;
std::string Application::shaderName = "pixel";

f32 vertexs[] =
{
     1.0,  1.0,
    -1.0,  1.0,
     1.0, -1.0,
    -1.0, -1.0,
};

u32 indices[] =
{
    0, 1, 2,
    1, 3, 2,
};

static f32 width = 640;
static f32 height = 640;
static f32 zoom = 4;
static f32 cx = 0;
static f32 cy = 0;
static f32 mouse_x = 0;
static f32 mouse_y = 0;

inline bool file_exist(const std::string& path) {
    std::ifstream file(path);
    return file.is_open();
}

inline bool shader_path_exist(const std::string& name) {
    return file_exist(SHADER_SOURCE_DIRECTORY + name + ".vert") && file_exist(SHADER_SOURCE_DIRECTORY + name + ".frag");
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    ::width = width;
    ::height = height;
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)window;
    (void)scancode;
    (void)mods;
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && Application::currentShader) {
        if (shader_path_exist(Application::shaderName)) {
            Application::loadShader(Application::shaderName);
        } else {
            Application::loadShader(DEFAULT_SHADER_NAME);
        }
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    (void)window;
    (void)xoffset;
    zoom -= yoffset * 0.5f;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    (void)window;
    mouse_x = xpos;
    mouse_y = ypos;
}

Application::Application() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(640, 640, "glsl test", NULL, NULL);

    glfwMakeContextCurrent(m_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetKeyCallback(m_window, key_callback);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
    glfwSetScrollCallback(m_window, scroll_callback);
    glfwSetCursorPosCallback(m_window, mouse_callback);

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    GLCALL(glEnable(GL_BLEND));
    GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    m_imguiIO = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

Application::~Application() {
    delete currentShader;
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Application::loadShader(const std::string& name) {
    currentShader->clearShaderError();
    currentShader->release();
    currentShader->create();
    currentShader->attach_shader(GL_VERTEX_SHADER, SHADER_SOURCE_DIRECTORY + name + ".vert");
    currentShader->attach_shader(GL_FRAGMENT_SHADER, SHADER_SOURCE_DIRECTORY + name + ".frag");
    if (!currentShader->getShaderError().empty()) {
        currentShader->release();
        currentShader->create();
        currentShader->attach_shader(GL_VERTEX_SHADER, SHADER_SOURCE_DIRECTORY DEFAULT_SHADER_NAME ".vert");
        currentShader->attach_shader(GL_FRAGMENT_SHADER, SHADER_SOURCE_DIRECTORY DEFAULT_SHADER_NAME ".frag");
    }
    currentShader->link();
    shaderName = name;
}

static char buf[64] = "test";

void Application::run() {
    VertexArray vao;
    VertexBuffer vbo(vertexs, sizeof(f32) * 8);
    IndexBuffer ibo(indices, 6);

    VertexBufferLayout layout;
    layout.add<float>(2);
    vao.apply_buffer_layout(layout);

    currentShader = new ShaderProgram();
    if (shader_path_exist(shaderName)) {
        loadShader(shaderName);
    } else {
        loadShader(DEFAULT_SHADER_NAME);
    }

    glClearColor(0.1, 0.1, 0.1, 1);

    while(!glfwWindowShouldClose(m_window))
    {
        GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        
        currentShader->bind();
        vao.bind();
        currentShader->set_2f("resolution", width, height);
        currentShader->set_2f("mouse_coord", mouse_x / width, 1 - mouse_y / height);
        currentShader->set_2f("center", cx, cy);
        currentShader->set_1f("time", glfwGetTime());
        currentShader->set_1f("zoom", zoom);
        GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

        float speed = 0.02;
        if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            speed  *= 2;
        if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
            cy -= speed;
        if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
            cy += speed;
        if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
            cx -= speed;
        if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
            cx += speed;


        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("status");
        ImGui::InputText("shader", buf, IM_ARRAYSIZE(buf));
        ImGui::SameLine();
        if (ImGui::Button("load")) {
            if (shader_path_exist(buf)) {
                loadShader(buf);
            } else {
                loadShader(DEFAULT_SHADER_NAME);
            }
        }

        ImGui::SliderFloat("zoom", &zoom, 0.1, 10);

        ImGui::TextColored(ImVec4(1,1,0,1), "message");
        ImGui::BeginChild("Scrolling");
        for (auto& error : currentShader->getShaderError()) {
            ImGui::Text(error.c_str(), "");
        }
        ImGui::EndChild();       

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

