#ifndef _WINDOW_H
#define _WINDOW_H

#include "Renderer.hpp"

class Window {
public:
    Window() {}
    Window(const std::string& title, i32 width, i32 height);
    Window(const Window&) = delete;
    ~Window();
    bool close() const;
    void swap_buffers();
    void set_ortho(const glm::vec2& w, const glm::vec2& h, const glm::vec2& r);
    void set_window_user_pointer(void* user);

    struct {
        struct {
            i32 button, action, mods;
            f64 xoffset, yoffset;
        } mouse;
        struct {
            f64 x, y;
        } cursor;
        struct {
            i32 key, scancode, action, mods;
        } keyboard;
    } input;

    GLFWwindow* win;

    i32 width, height;
    glm::vec2 pos;
    glm::mat4 ortho;

private:
    void update_window_size(GLFWwindow* window, int width, int height);
    void update_key(GLFWwindow* window, int key, int scancode, int action, int mods);
    void update_cursor_pos(GLFWwindow* window, double xpos, double ypos);
    void update_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset);
    void update_mouse_button(GLFWwindow* window, int button, int action, int mods);

};

#endif
