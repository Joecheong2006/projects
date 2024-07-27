#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cglm/cglm.h>
#include "camera.h"
#include "string.h"
#include "opengl_object.h"
#include "sprite.h"
#include "stb_image.h"
#include <string.h>

#include "camera_shake.h"

#include "anim_duration_system.h"

#include "game_object_system.h"

#include "memallocate.h"

#include "audio.h"

#include "debug/line_renderer.h"

#define PI 3.14159265359

#define WIDTH 640
#define HEIGHT 640

typedef void(*input_control_key_callback)(void* owner, int key, int scancode, int action, int mods);
typedef void(*input_control_cursor_pos_callback)(void* owner, double xpos, double ypos);
typedef void(*input_control_mouse_button_callback)(void* owner, int button, int action, int mods);

typedef struct {
    input_control_key_callback key_callback;
    input_control_cursor_pos_callback cursor_pos_callback;
    input_control_mouse_button_callback mouse_button_callback;
} input_control;

typedef void(*window_control_render_callback)(void* owner);
typedef void(*window_control_resize_callback)(void* window, int width, int height);
typedef struct {
    window_control_render_callback render_callback;
    window_control_resize_callback resize_callback;
} window_control;

typedef struct {
    void* owner;
    input_control input;
    window_control window;
} callback_controller;

typedef struct {
    GLFWwindow* window;
    int width, height;
} window_state;

void close_application(window_state* window) {
    glfwSetWindowShouldClose(window->window, GLFW_TRUE);
}

typedef struct {
    window_state win_state;
} Game;

void game_window_resize_callback(void* owner, int width, int height) {
    Game* game = owner;
    glViewport(0, 0, width, height);

    camera* cam = find_game_object_by_index(0)->self;
    set_camera_ortho_mat4(cam->projection, (vec2){width, height});

    game->win_state.width = width;
    game->win_state.height = height;
}

void game_key_callback(void* owner, int key, int scancode, int action, int mods) {
    (void)scancode, (void)mods;
    Game* game = owner;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        close_application(&game->win_state);
    }

    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        static int line_mode = 0;
        glPolygonMode(GL_FRONT_AND_BACK, (line_mode = !line_mode) ? GL_LINE : GL_FILL);
    }

    static camera_shake_object cam_shake;
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        cam_shake = (camera_shake_object){ .duration = 0.2, .strength = 0.03 };
        create_camera_shake(&cam_shake);
    }
}

void game_cursor_pos_callback(void* owner, double xpos, double ypos) {
    Game* game = owner;
    vec4 uv = { xpos / game->win_state.width * 2 - 1, (1 - ypos / game->win_state.height) * 2 - 1, 0, 1};
    mat4 m;

    camera* cam = find_game_object_by_index(0)->self;
    glm_mat4_mul(cam->projection, cam->view, m);
    glm_mat4_inv(m, m);
    glm_mat4_mulv(m, uv, uv);
}

void game_mouse_button_callback(void* owner, int button, int action, int mods) {
}

void game_render_callback(void* owner) {
    Game* game = owner;
    camera* cam = find_game_object_by_index(0)->self;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    callback_controller* c = glfwGetWindowUserPointer(window);
    if (c->window.resize_callback) {
        c->window.resize_callback(c->owner, width, height);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    callback_controller* c = glfwGetWindowUserPointer(window);
    if (c->input.key_callback) {
        c->input.key_callback(c->owner, key, scancode, action, mods);
    }
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    callback_controller* c = glfwGetWindowUserPointer(window);
    if (c->input.cursor_pos_callback) {
        c->input.cursor_pos_callback(c->owner, xpos, ypos);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    callback_controller* c = glfwGetWindowUserPointer(window);
    if (c->input.mouse_button_callback) {
        c->input.mouse_button_callback(c->owner, button, action, mods);
    }
}

void sound_test(const char* path, float pitch, float gain) {
    audio_context audio;
    init_audio(&audio);

    float listenerOri[] = { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
    set_audio_listener_properties((vec3){0, 0, 0}, (vec3){0, 0, 0}, listenerOri);

    u32 source = create_audio_source(pitch, gain, (vec3){0, 0, 0}, (vec3){0, 0, 0}, 0);

    u32 buffer = gen_sound_buffer(path);
    if (!buffer) {
        printf("load %s failed\n", path);
        alDeleteSources(1, &source);
        shutdown_audio(&audio);
        return;
    }

    alSourcei(source, AL_BUFFER, buffer);
    al_check_error();

    alSourcePlay(source);
    al_check_error();

    ALint source_state;
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    al_check_error();
    while (source_state == AL_PLAYING) {
        alGetSourcei(source, AL_SOURCE_STATE, &source_state);
        al_check_error();
    }

    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    shutdown_audio(&audio);
}

int main(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    stbi_set_flip_vertically_on_load(1);

    GLFWwindow* app_window = glfwCreateWindow(WIDTH, HEIGHT, "chess", NULL, NULL);
    glfwSwapInterval(1);

    glfwSetKeyCallback(app_window, key_callback);
    glfwSetFramebufferSizeCallback(app_window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(app_window, mouse_button_callback);
    glfwSetCursorPosCallback(app_window, cursor_pos_callback);
    
    glfwMakeContextCurrent(app_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    printf("Opengl Version %s\n", glGetString(GL_VERSION));

    GLC(glEnable(GL_DEPTH_TEST));
    GLC(glEnable(GL_BLEND));
    GLC(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLC(glDepthFunc(GL_LESS));

    audio_context audio;
    init_audio(&audio);

    set_audio_listener_properties((vec3){0, 0, 0}, (vec3){0, 0, 0}, (f32[]){ 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f });

    // setting up
    setup_anim_system();
    init_sprite_instance();
    init_debug_line_renderer_instance();

    Game game;
    game.win_state.window = app_window;

    callback_controller con = (callback_controller){
        .owner = &game,
        .input.cursor_pos_callback = game_cursor_pos_callback,
        .input.key_callback = game_key_callback,
        .input.mouse_button_callback = game_mouse_button_callback,
        .window.resize_callback = game_window_resize_callback,
        .window.render_callback = game_render_callback,
    };

    glfwSetWindowUserPointer(app_window, &con);

    camera cam;
    init_camera(&cam, (vec2){WIDTH, HEIGHT});

    game.win_state.width = WIDTH;
    game.win_state.height = HEIGHT;
    
    glClearColor(0.0, 0.1, 0.1, 0);

    init_game_object_system();
    create_game_object(&(game_object){
        .self = &cam,
        .on_start = NULL,
        .on_activate = NULL,
        .on_update = NULL,
        .on_render = NULL,
        .on_destory = NULL,
    });

    float pitch = 1, gain = 1;
    u32 buffers[2];
    u32 sources[2];

    {
        sources[0] = create_audio_source(pitch, gain, (vec3){0, 0, 0}, (vec3){0, 0, 0}, 0);
        char* audio_file_path = "assets/audio/minecraft1.mp3";
        buffers[0] = gen_sound_buffer(audio_file_path);
        if (!buffers[0]) {
            printf("load %s failed\n", audio_file_path);
            alDeleteSources(1, &sources[0]);
            shutdown_audio(&audio);
            exit(1);
        }

        alSourcei(sources[0], AL_BUFFER, buffers[0]);
        alSourcePlay(sources[0]);
    }
    {
        sources[1] = create_audio_source(pitch, gain, (vec3){0, 0, 0}, (vec3){0, 0, 0}, 0);
        char* audio_file_path = "assets/audio/killshot-speedup.mp3";
        buffers[1] = gen_sound_buffer(audio_file_path);
        if (!buffers[1]) {
            printf("load %s failed\n", audio_file_path);
            alDeleteSources(1, &sources[1]);
            shutdown_audio(&audio);
            exit(1);
        }

        alSourcei(sources[1], AL_BUFFER, buffers[1]);
        // alSourcePlay(sources[1]);
    }

    sprite_texture sp_tex = {
        .per_sprite = {48.0 / 192, 48.0 / 192},
    };
    init_texture(&sp_tex.tex, "assets/Sprout-Lands/Characters/Basic-Charakter.png", TextureFilterNearest);

    sprite sp = {
        .sprite_index = {0, 0},
        .color = {1, 1, 1, 1}
    };

    while(!glfwWindowShouldClose(app_window))
    {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float step = 1.0 / 144 * 4;
        if (glfwGetKey(app_window, GLFW_KEY_UP) == GLFW_PRESS) {
            translate_camera(&cam, (vec3){0, step, 0});
        }
        else if (glfwGetKey(app_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            translate_camera(&cam, (vec3){0, -step, 0});
        }
        if (glfwGetKey(app_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            translate_camera(&cam, (vec3){-step, 0, 0});
        }
        else if (glfwGetKey(app_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            translate_camera(&cam, (vec3){step, 0, 0});
        }

		update_anim_system();
		con.window.render_callback(con.owner);
        update_game_object_system();

        float points[] = { 0, 0, 0, 2, 2, 0, };
        render_debug_line(points, (vec3){0, 0, 1});

        transform tran = {
            .position = {0, 0, 1},
            .scale = {4, 4, 1},
            .parent = NULL,
        };
        static int index = 0;
        index = (int)(glfwGetTime() * 7) % 16;
        sp.sprite_index[0] = index % 4;
        sp.sprite_index[1] = (int)(index / 4);

        render_sprite(&cam, &tran, &sp_tex, &sp);

        glfwSwapBuffers(app_window);
        glfwPollEvents();
    }

    glDeleteProgram(sprite_instance.shader);
    shutdown_game_object_system();
    shutdown_anim_system();

    alDeleteBuffers(1, &sources[0]);
    alDeleteBuffers(1, &sources[1]);
    alDeleteBuffers(1, &buffers[0]);
    alDeleteBuffers(1, &buffers[1]);
    shutdown_audio(&audio);

    glfwDestroyWindow(app_window);
    glfwTerminate();

    CHECK_MEMORY_LEAK();

    return 0;
}
