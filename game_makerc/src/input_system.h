#ifndef _INPUT_SYSTEM_
#define _INPUT_SYSTEM_
#include "basic/queue.h"

typedef struct {
    i32 key, scancode, action, mods;
} key_state;

typedef struct {
    queue(key_state) buffer;
} key_input;

typedef struct {
    i32 button, action, mods;
    f32 x, y;
} mouse_state;

typedef struct {
    queue(mouse_state) buffer;
    f64 x, y;
} mouse_input;

typedef struct GLFWwindow GLFWwindow;
typedef struct input_system input_system;

void setup_input_system(GLFWwindow* app_window);
void shutdown_input_system();

void update_input_key(i32 key, i32 scancode, i32 action, i32 mods);
key_state input_key_state();
i32 input_key_press(i32 key);
i32 input_key_release(i32 key);

void update_input_mouse_cursor(f64 x, f64 y);
void update_input_mouse_button(i32 button, i32 action, i32 mods);
mouse_state input_mouse_state();
void input_mouse_cursor(f32 cursor_pos[2]);
i32 input_mouse_button_down(i32 button);
i32 input_mouse_button_release(i32 button);

#endif
