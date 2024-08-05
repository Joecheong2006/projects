#ifndef _WINDOW_CALLBACK_H_
#define _WINDOW_CALLBACK_H_
#include "core/defines.h"

typedef struct {
    void* owner;
    struct {
        void(*on_key)(void* owner, i32 key, i32 scancode, i32 action, i32 mods);
        void(*on_cursor_pos)(void* owner, double xpos, double ypos);
        void(*on_mouse_button)(void* owner, i32 button, i32 action, i32 mods);
    } input;
    struct {
        void(*on_render)(void* owner);
        void(*on_resize)(void* window, i32 width, i32 height);
    } window;
} window_callback_setup;

typedef struct GLFWwindow GLFWwindow;
void setup_window_callback(GLFWwindow* window, window_callback_setup* setup);

#endif
