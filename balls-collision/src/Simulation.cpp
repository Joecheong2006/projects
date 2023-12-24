#include "Simulation.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <random>
#include <chrono>
#include <future>

#define UFRAME (1.0/144.0)
#define RFRAME (1.0/144.0)
#define BASE_SPEED 2000.0f

#define Timer(x, dt)                                                             \
  {                                                                              \
      auto start = std::chrono::system_clock::now();                             \
      x;                                                                         \
      auto end = std::chrono::system_clock::now();                               \
      std::chrono::duration<float> duration = end - start;                       \
      dt = duration.count() * 1000.0;                                            \
  }

std::vector<i32> get_range_dev(i32 range, i32 dev) {
    int l = range % dev;
    int s = int(range / dev);
    std::vector<int> result;
    result.push_back(0);
    for(int i = 0; i < dev; ++i)
    {
        if(l-- > 0)
        {
            result.push_back(result[i] + s + 1);
            continue;
        }
        result.push_back(result[i] + s);
    }
    return result;
}

Simulation Simulation::instance;

static glm::vec2 ws;
Simulation::Simulation()
    : m_window("collision test", 960, 640), m_renderer(), cm(), frame(UFRAME), pause(false)
{
    m_window.set_window_user_pointer(this);

    glfwSetKeyCallback(m_window.win, [](GLFWwindow* w, int k, int s, int a, int m) {
        static_cast<Simulation*>(glfwGetWindowUserPointer(w))->update_key(w, k, s, a, m);
    });

    glfwSetCursorPosCallback(m_window.win, [](GLFWwindow* w, f64 x, f64 y) {
        static_cast<Simulation*>(glfwGetWindowUserPointer(w))->update_cursor_pos(w, x, y);
    });

    glfwSetScrollCallback(m_window.win, [](GLFWwindow* w, f64 xoffset, f64 yoffset) {
        static_cast<Simulation*>(glfwGetWindowUserPointer(w))->update_mouse_scroll(w, xoffset, yoffset);
    });

    glfwSetFramebufferSizeCallback(m_window.win, [](GLFWwindow* w, int width, int height) {
        static_cast<Simulation*>(glfwGetWindowUserPointer(w))->update_window_size(w, width, height);
    });

    glfwSetMouseButtonCallback(m_window.win, [](GLFWwindow* w, int button, int action, int mods) {
        static_cast<Simulation*>(glfwGetWindowUserPointer(w))->update_mouse_button(w, button, action, mods);
    });

    glm::vec2 window_size = glm::vec2(m_window.width, m_window.height);
    box = Collision::Box(glm::vec2(0), window_size / 32.f);
    ws = window_size / 20.f;
    m_renderer.o = glm::ortho(-ws.x, ws.x, -ws.y, ws.y, -1.0f, 1.0f);

    cm.create_circle(mouse_pos, glm::vec3(3, 3, 3), 1);
}

void Simulation::solve_collision(i32 begin, i32 end) {
    auto& objs = cm.entities;
    u64 len = cm.entities.size();
    for(i32 i = begin; i < end; ++i) {
        for(u64 j = 0; j < len; ++j) {
            Collision::Colliable<Circle>* d;
            d = static_cast<Collision::Colliable<Circle>*>(&objs[i]);
            if(d->collide(objs[j]))
                d->solve_collision(objs[j]);
        }
    }
}

bool spown = false;
void Simulation::update_physics(f32 frame) {
    static u64 counter = 0;
    if(spown)
    {
        static std::random_device dev;
    
        static std::mt19937 rng(dev());
        static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        static f32 d = 0.4;
        static f32 s = 30.f;

        ++counter;
        if(counter >= (1.0f / frame) / 30)
        {
            glm::vec2 p = box.wah;
            instance.cm.create_circle(glm::vec2(-p.x+1, p.y-1), glm::vec3(dist(dev) + 0.1, dist(dev) + 0.4, 2 + dist(dev)), d).add_force(glm::vec2(BASE_SPEED * s, 0));
            instance.cm.create_circle(glm::vec2(-p.x+1, p.y-2), glm::vec3(dist(dev) + 0.1, dist(dev) + 0.4, 2 + dist(dev)), d).add_force(glm::vec2(BASE_SPEED * s, 0));
            instance.cm.create_circle(glm::vec2(-p.x+1, p.y-3), glm::vec3(dist(dev) + 0.1, dist(dev) + 0.4, 2 + dist(dev)), d).add_force(glm::vec2(BASE_SPEED * s, 0));
            counter = 0;
        }
    }

    auto& objs = cm.entities;
    for(u64 i = 1; i < objs.size(); ++i) {
        if(objs[i].m_pos.y - objs[i].d < box.pos.y - box.wah.y) {
            objs[i].m_pos.y = (box.pos.y - box.wah.y + objs[i].d);
        }
        else if(objs[i].m_pos.y + objs[i].d > box.pos.y + box.wah.y) {
            objs[i].m_pos.y = (box.pos.y + box.wah.y - objs[i].d);
        }
        if(objs[i].m_pos.x - objs[i].d < box.pos.x - box.wah.x) {
            objs[i].m_pos.x = (box.pos.x - box.wah.x + objs[i].d);
        }
        else if(objs[i].m_pos.x + objs[i].d > box.pos.x + box.wah.x) {
            objs[i].m_pos.x = (box.pos.x + box.wah.x - objs[i].d);
        }
    }

    //solve_collision(1, cm.entities.size());
    const i32 dev = 2;
    std::vector<std::future<void>> ml;
    auto range = get_range_dev(cm.entities.size(), dev);
    range[0]++;
    for(i32 i = 0; i < dev; ++i)
        ml.push_back(std::async(std::launch::async, &Simulation::solve_collision, this, range[i], range[i+1]));
    for(auto& e : ml)
        e.wait();
    for(auto& e : cm.entities)
    {
        e.add_force(glm::vec2(0, -98.1));
        e.update(frame);
    }
}

void Simulation::update_sub_step_physics(i32 sub_step) {
    const f32 sub_frame = frame / sub_step;
    for(int i = sub_step; i > 0; --i)
        update_physics(sub_frame);
}

static bool control = false;

void Simulation::update() {
    if(!pause)
        update_sub_step_physics(4);
    cm.entities[0].m_pos = mouse_pos;
}

void Simulation::render() {
    m_renderer.draw_circle(cm);
}

void Simulation::Run() {
    instance.m_renderer.set_background_color(glm::vec3(0.1, 0.1, 0.1));
    f32 lastUpdateTime = glfwGetTime();
    f32 lastFrameTime = lastUpdateTime;

    while(!instance.m_window.close())
    {
        f32 current = glfwGetTime();
        instance.m_renderer.clear();

        f32 update_frame = current - lastUpdateTime;
        if(update_frame >= UFRAME)
        {
            instance.update();
            instance.frame = update_frame;
            lastUpdateTime = current;
        }

        f32 render_frame = current - lastFrameTime;
        if(render_frame >= RFRAME)
        {
            f32 rf;
            Timer(
                instance.render();
                instance.m_window.swap_buffers();
                , rf);
            lastFrameTime = current;
            char buf[100];
            snprintf(buf, 100, "render: %.3fms, logic: %.3fms, objects: %llu", rf, 1000 * instance.frame, instance.cm.entities.size() - 1);
            glfwSetWindowTitle(instance.m_window.win, buf);
        }
        glfwPollEvents();
    }
}

void Simulation::update_window_size(GLFWwindow* window, int width, int height) {
    (void)window;
    glViewport(0, 0, width, height);
    instance.box.wah = glm::vec2(width, height) / 32.0f;
    //instance.box.wah = glm::vec2(width, height) / 20.0f;
}

void Simulation::update_mouse_button(GLFWwindow* window, int button, int action, int mods) {
    static std::random_device dev;
    static std::mt19937 rng(dev());
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    (void)window;
    (void)mods;

    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        cm.create_circle(mouse_pos - glm::vec2(0,  0.5 + 2 * cm.entities[0].d), glm::vec3(dist(dev) + 0.1, dist(dev) + 0.1, 2 + dist(dev)), cm.entities[0].d);
}

void Simulation::update_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;

    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if(key == GLFW_KEY_S && action == GLFW_PRESS)
        spown = !spown;

    if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        pause = !pause;

    if((key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) && action == GLFW_REPEAT)
        control = true;
    else
        control = false;
}

void Simulation::update_cursor_pos(GLFWwindow* window, double xpos, double ypos) {
    (void)window;
    mouse_pos.x = xpos / 10 - (f32)m_window.width / 20;
    mouse_pos.y = (f32)m_window.height / 20 - ypos / 10;
}

void Simulation::update_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset) {
    (void)window;
    (void)xoffset;

    if(control)
    {
        ws -= yoffset;
        m_renderer.o = glm::ortho(-ws.x, ws.x, -ws.y, ws.y, -1.0f, 1.0f);
        return;
    }

    cm.entities[0].d -= yoffset * 0.1;
    if(cm.entities[0].d <= 0.3)
        cm.entities[0].d = 0.3;
}
