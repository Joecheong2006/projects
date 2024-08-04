#include "input_system.h"
#include <GLFW/glfw3.h>
#include "core/assert.h"

static struct {
    GLFWwindow* app_window;
    key_input key;
    mouse_input mouse;
} instance;

void setup_input_system(GLFWwindow* app_window) {
    instance.app_window = app_window;
    instance.key.buffer = make_queue(32, sizeof(key_state));
    instance.mouse.buffer = make_queue(32, sizeof(mouse_state));
}

void shutdown_input_system() {
    instance.app_window = NULL;
    free_queue(instance.key.buffer);
    free_queue(instance.mouse.buffer);
}

void update_input_key(i32 key, i32 scancode, i32 action, i32 mods) {
    if (queue_is_full(instance.key.buffer)) {
        queue_pop(instance.key.buffer);
    }
    queue_push(instance.key.buffer, key, scancode, action, mods);
}

key_state input_key_state() {
    key_state result = {
        .key = -1,
    };
    key_state* front = queue_front(instance.key.buffer);
    if (front) {
        result = *front;
        queue_pop(instance.key.buffer);
    }
    return result;
}

i32 input_key_press(i32 key) {
    ASSERT(instance.app_window != NULL);
    return glfwGetKey(instance.app_window, key) == GLFW_PRESS;
}

i32 input_key_release(i32 key) {
    ASSERT(instance.app_window != NULL);
    return glfwGetKey(instance.app_window, key) == GLFW_RELEASE;
}

void update_input_mouse_cursor(f64 x, f64 y) {
    instance.mouse.x = x;
    instance.mouse.y = y;
}

void update_input_mouse_button(i32 button, i32 action, i32 mods) {
    if (queue_is_full(instance.mouse.buffer)) {
        queue_pop(instance.mouse.buffer);
    }
    queue_push(instance.mouse.buffer, button, action, mods, instance.mouse.x, instance.mouse.y);
}

mouse_state input_mouse_state() {
    mouse_state result = {
        .button = -1,
    };
    mouse_state* front = queue_front(instance.mouse.buffer);
    if (front) {
        result = *front;
        queue_pop(instance.mouse.buffer);
    }
    return result;
}

void input_mouse_cursor(f32 cursor_pos[2]) {
    cursor_pos[0] = instance.mouse.x;
    cursor_pos[1] = instance.mouse.y;
}

i32 input_mouse_button_down(i32 button) {
    return glfwGetMouseButton(instance.app_window, button) == GLFW_PRESS;
}

i32 input_mouse_button_release(i32 button) {
    return glfwGetMouseButton(instance.app_window, button) == GLFW_RELEASE;
}

