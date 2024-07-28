#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cglm/cglm.h>
#include "camera.h"
#include "cglm/vec2.h"
#include "string.h"
#include "opengl_object.h"
#include "sprite.h"
#include "stb_image.h"
#include <string.h>

#include "camera_shake.h"

#include "anim_duration_system.h"
#include "anim_position_slide.h"

#include "game_object_system.h"

#include "memallocate.h"

#include "audio.h"

#include "debug/line_renderer.h"

#include "physics/rigid2d.h"
#include "physics/collider2d.h"
#include "physics/box2d.h"
#include "physics/circle2d.h"

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
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
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

void render_transform_outline(transform* tran, vec3 color) {
    vec2 bottom_left = { -tran->scale[0] * 0.5, -tran->scale[1] * 0.5 };
    vec2 top_right = { tran->scale[0] * 0.5, tran->scale[1] * 0.5 };

    float points[6];
    glm_vec3_add(tran->position, bottom_left, points);
    glm_vec3_add(points, (vec3){tran->scale[0], 0, 0}, points + 3);
    render_debug_line(points, color);

    glm_vec3_add(tran->position, bottom_left, points);
    glm_vec3_add(points, (vec3){0, tran->scale[1], 0}, points + 3);
    render_debug_line(points, color);

    glm_vec3_add(tran->position, top_right, points);
    glm_vec3_add(points, (vec3){-tran->scale[0], 0, 0}, points + 3);
    render_debug_line(points, color);

    glm_vec3_add(tran->position, top_right, points);
    glm_vec3_add(points, (vec3){0, -tran->scale[1], 0}, points + 3);
    render_debug_line(points, color);
}

void resolve_velocity(collision2d_state* state, rigid2d* r1, rigid2d* r2) {
    const f32 total_inverse_mass = 1.0 / (r1->inverse_mass + r2->inverse_mass);
    if (r2->is_static) {
        glm_vec2_mulsubs(state->normal, state->depth, r1->tran->position);
    }
    else if (r1->is_static) {
        glm_vec2_muladds(state->normal, state->depth, r2->tran->position);
    }
    else {
        glm_vec2_mulsubs(state->normal, state->depth * r1->inverse_mass * total_inverse_mass, r1->tran->position);
        glm_vec2_muladds(state->normal, state->depth * r2->inverse_mass * total_inverse_mass, r2->tran->position);
    }

    vec2 separate_v;
    glm_vec2_sub(r2->v, r1->v, separate_v);
    const f32 separate_normal = glm_vec2_dot(separate_v, state->normal);
    if (separate_normal > 0) {
        return;
    }

    const f32 e = glm_min(r1->restitution, r2->restitution);
    const f32 J = separate_normal * (e + 1.0) * total_inverse_mass;

    glm_vec2_muladds(state->normal, (1.0 - r1->is_static) * J * r1->inverse_mass, r1->v);
    glm_vec2_mulsubs(state->normal, (1.0 - r2->is_static) * J * r2->inverse_mass, r2->v);
}

void sprite_index_anim(anim_position_slide* slide, float dur) {
    const int key_frames = 2;
    int index = dur * key_frames;
    float* sprite_index = *slide->target;
    sprite_index[0] = index % 2;
    sprite_index[1] = 0;
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
        char* audio_file_path = "assets/audio/yippee.mp3";
        buffers[1] = gen_sound_buffer(audio_file_path);
        if (!buffers[1]) {
            printf("load %s failed\n", audio_file_path);
            alDeleteSources(1, &sources[1]);
            shutdown_audio(&audio);
            exit(1);
        }

        alSourcei(sources[1], AL_BUFFER, buffers[1]);
        alSourcePlay(sources[1]);
    }

    sprite_texture sp_tex;
    init_texture(&sp_tex.tex, "assets/Sprout-Lands/Characters/Basic-Charakter.png", TextureFilterNearest);
    glm_vec2_copy((vec2){48.0 / sp_tex.tex.width, 48.0 / sp_tex.tex.height}, sp_tex.per_sprite);

    sprite sp = {
        .sprite_index = {0, 0},
        .color = {1, 1, 1, 1}
    };
    transform tran = {
        .position = {0, 0, 1},
        .scale = {2, 2, 1},
        .parent = NULL,
    };
    rigid2d rig;
    init_rigid2d(&rig, &tran);
    rigid2d_set_mass(&rig, 0.01);

    anim_position_slide sprite_anim;
    init_anim_position_slide(&sprite_anim, (vec3){4, 0, 0}, sprite_index_anim);
    set_anim_position_slide(&sprite_anim, &sp.sprite_index);
    anim_duration anim = { .loop = 1 };
    init_anim_position_slide_duration(&anim, &sprite_anim, 0.5);
    create_anim_duration(&anim);

    transform tran1;
    init_transform(&tran1);
    rigid2d r1;
    init_rigid2d(&r1, &tran1);
    // glm_vec2_copy((vec2){9.81, 0}, r1.g);
    glm_vec2_copy((vec2){0, 0}, r1.g);
    r1.restitution = 1;
    r1.v[0] = 3;
    rigid2d_set_mass(&r1, 0.2);
    circle2d context1 = {
        .center = {0, 0}, .radius = 0.5
    };
    collider2d collider1 = create_collider2d(ColliderCircle2d, &r1, &context1);

    transform tran2;
    init_transform(&tran2);
    tran2.position[1] = 0.8;
    tran2.position[0] = 2;
    rigid2d r2;
    init_rigid2d(&r2, &tran2);
    glm_vec2_copy((vec2){0, 0}, r2.g);
    r2.restitution = 0.7;
    circle2d context2 = {
        .center = {0, 0}, .radius = 0.5
    };
    collider2d collider2 = create_collider2d(ColliderCircle2d, &r2, &context2);

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

        // static int index = 0;
        // index = (int)(glfwGetTime() * 5) % 16;
        // sp.sprite_index[0] = index % 4;
        // sp.sprite_index[1] = (int)(index / 4);

        const float dt = 1.0 / 144;
        // rig.process(&rig, dt);

        render_sprite(&cam, &tran, &sp_tex, &sp);
        render_transform_outline(&tran, (vec3){1, 1, 1});

        collision2d_state state = collider1.collide(&collider1, &collider2);

        vec3 outline_color1 = {1, 1, 1};
        vec3 outline_color2 = {1, 1, 1};

        if (state.depth > 0) {
            outline_color1[0] = 0;
            outline_color1[2] = 0;
            resolve_velocity(&state, collider1.parent, collider2.parent);
        }

        r1.process(&r1, dt);
        r2.process(&r2, dt);

        render_transform_outline(&tran1, outline_color1);
        render_transform_outline(&tran2, outline_color2);

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
