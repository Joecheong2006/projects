#include "Window.h"

#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Simulation.h"

Window::Window(const std::string& title, i32 width, i32 height) {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    win = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

    glfwMakeContextCurrent(win);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    stbi_set_flip_vertically_on_load(true);

    GLCALL(glEnable(GL_BLEND));
    GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    glfwSwapInterval(0);

    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glfwSetKeyCallback(win, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(w));
        window->input.keyboard.key = key;
        window->input.keyboard.scancode = scancode;
        window->input.keyboard.action = action;
        window->input.keyboard.mods = mods;
        window->update_key(w, key, scancode, action, mods);
    });

    glfwSetCursorPosCallback(win, [](GLFWwindow* w, f64 x, f64 y) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(w));
        window->pos.x = x;
        window->pos.y = y;
        window->update_cursor_pos(w, x, y);
    });

    glfwSetScrollCallback(win, [](GLFWwindow* w, f64 xoffset, f64 yoffset) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(w));
        window->input.mouse.xoffset = xoffset;
        window->input.mouse.yoffset  = yoffset;
        window->update_mouse_scroll(w, xoffset, yoffset);
    });

    glfwSetFramebufferSizeCallback(win, [](GLFWwindow* w, int width, int height) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(w));
        window->width = width;
        window->height = height;
        window->update_window_size(w, width, height);
    });

    glfwSetMouseButtonCallback(win, [](GLFWwindow* w, int button, int action, int mods) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(w));
        window->input.mouse.button = button;
        window->input.mouse.action = action;
        window->input.mouse.mods = mods;
        window->update_mouse_button(w, button, action, mods);
    });

    this->width = width;
    this->height = height;

}

Window::~Window() {
    glfwDestroyWindow(win);
    glfwTerminate();
}

bool Window::close() const {
    return glfwWindowShouldClose(win);
}

void Window::swap_buffers() {
    glfwSwapBuffers(win);
}

void Window::set_ortho(const glm::vec2& w, const glm::vec2& h, const glm::vec2& r) {
   ortho = glm::ortho(w.x, w.y, h.x, h.y, r.x, r.y); 
}

void Window::set_window_user_pointer(void* user) {
    glfwSetWindowUserPointer(win, user);
}

void Window::update_window_size(GLFWwindow* window, int width, int height) {
    (void)window;
    (void)width;
    (void)height;
}

void Window::update_mouse_button(GLFWwindow* window, int button, int action, int mods) {
    (void)window;
    (void)button;
    (void)action;
    (void)mods;
}

void Window::update_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)window;
    (void)key;
    (void)scancode;
    (void)action;
    (void)mods;
}

void Window::update_cursor_pos(GLFWwindow* window, double xpos, double ypos) {
    (void)window;
    (void)xpos;
    (void)ypos;
}

void Window::update_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset) {
    (void)window;
    (void)xoffset;
    (void)yoffset;
}
