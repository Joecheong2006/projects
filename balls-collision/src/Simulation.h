#ifndef _SIMULATION_H
#define _SIMULATION_H

#include "Window.h"
#include "CollisionBox.h"

class Simulation {
    friend class Window;
public:
    static Simulation instance;

private:
    Window m_window;
    Renderer m_renderer;
    Circle::Manager cm;
    Collision::Box box;

    glm::vec2 mouse_pos;
    f32 frame;
    bool pause;

private:
    void update();
    void update_sub_step_physics(i32 sub_step = 1);
    void update_physics(f32 frame);
    void render();

    void solve_collision(i32 begin, i32 end);

public:
    Simulation();

public:
    static void Run();
    //static void window_pos_callback(GLFWwindow* window, int xpos, int ypos);

private:
    void update_window_size(GLFWwindow* window, int width, int height);
    void update_key(GLFWwindow* window, int key, int scancode, int action, int mods);
    void update_cursor_pos(GLFWwindow* window, double xpos, double ypos);
    void update_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset);
    void update_mouse_button(GLFWwindow* window, int button, int action, int mods);

};

#endif
