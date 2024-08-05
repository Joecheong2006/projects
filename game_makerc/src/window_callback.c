#include "window_callback.h"
#include "input_system.h"
#include <GLFW/glfw3.h>

static void framebuffer_size_callback(GLFWwindow* window, i32 width, i32 height) {
    window_callback_setup* c = glfwGetWindowUserPointer(window);
    if (c->window.on_resize) {
        c->window.on_resize(c->owner, width, height);
    }
}

static void key_callback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
    update_input_key(key, scancode, action, mods);

    window_callback_setup* c = glfwGetWindowUserPointer(window);
    if (c->input.on_key) {
        c->input.on_key(c->owner, key, scancode, action, mods);
    }
}

static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    update_input_mouse_cursor(xpos, ypos);

    window_callback_setup* c = glfwGetWindowUserPointer(window);
    if (c->input.on_cursor_pos) {
        c->input.on_cursor_pos(c->owner, xpos, ypos);
    }

}

static void mouse_button_callback(GLFWwindow* window, i32 button, i32 action, i32 mods) {
    update_input_mouse_button(button, action, mods);

    window_callback_setup* c = glfwGetWindowUserPointer(window);
    if (c->input.on_mouse_button) {
        c->input.on_mouse_button(c->owner, button, action, mods);
    }
}

void setup_window_callback(GLFWwindow* window, window_callback_setup* setup) {
    glfwSetWindowUserPointer(window, setup);

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
}
