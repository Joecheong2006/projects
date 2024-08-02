#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cglm/cglm.h>
#include <string.h>

#include "camera.h"
#include "cglm/vec2.h"
#include "string.h"
#include "opengl_object.h"
#include "sprite.h"
#include "stb_image.h"

#include "trace_info.h"

#include "input_system.h"

#include "camera_shake.h"

#include "anim_duration_system.h"
#include "anim_position_slide.h"

#include "game_object_system.h"

#include "basic/memallocate.h"

#include "audio.h"
#include "debug/primitive_shape_renderer.h"

#include "physics2d_object_system.h"
#include "physics2d/rigid2d.h"
#include "physics2d/collider2d.h"
#include "physics2d/box2d.h"
#include "physics2d/circle2d.h"
#include "physics2d/capsule2d.h"

#define PI 3.14159265359

#define WIDTH 640
#define HEIGHT 640

typedef void(*input_control_key_callback)(void* owner, i32 key, i32 scancode, i32 action, i32 mods);
typedef void(*input_control_cursor_pos_callback)(void* owner, double xpos, double ypos);
typedef void(*input_control_mouse_button_callback)(void* owner, i32 button, i32 action, i32 mods);

typedef struct {
    input_control_key_callback key_callback;
    input_control_cursor_pos_callback cursor_pos_callback;
    input_control_mouse_button_callback mouse_button_callback;
} input_control;

typedef void(*window_control_render_callback)(void* owner);
typedef void(*window_control_resize_callback)(void* window, i32 width, i32 height);
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
    i32 width, height;
} window_state;

void close_application(window_state* window) {
    glfwSetWindowShouldClose(window->window, GLFW_TRUE);
}

typedef struct {
    window_state win_state;
} Game;

void game_window_resize_callback(void* owner, i32 width, i32 height) {
    glViewport(0, 0, width, height);
    Game* game = owner;

    camera* cam = find_game_object_by_index(0)->self;
    cam->resolution[0] = width;
    cam->resolution[1] = height;

    game->win_state.width = width;
    game->win_state.height = height;
    
#if 0
    cam->persp = (camera_persp_state){
        .fov = glm_rad(45),
        .aspect = cam->resolution[0] / cam->resolution[1],
        .near = 0.1,
        .far = 100
    };
    cam->position[2] = 5;
    glm_lookat(cam->position, (vec3){0, 0, -1}, (vec3){0, 1, 0}, cam->view);
    set_camera_persp_mat4(cam);
#else
    set_camera_ortho_mat4(cam);
#endif
}

void game_key_callback(void* owner, i32 key, i32 scancode, i32 action, i32 mods) {
    (void)scancode, (void)mods;
    Game* game = owner;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        close_application(&game->win_state);
    }

    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        static i32 line_mode = 0;
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

void game_mouse_button_callback(void* , i32 button, i32 action, i32 mods) {
}

void game_render_callback(void* owner) {
    Game* game = owner;
    camera* cam = find_game_object_by_index(0)->self;
}

void framebuffer_size_callback(GLFWwindow* window, i32 width, i32 height) {
    callback_controller* c = glfwGetWindowUserPointer(window);
    if (c->window.resize_callback) {
        c->window.resize_callback(c->owner, width, height);
    }
}

void key_callback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
    update_input_key(key, scancode, action, mods);

    callback_controller* c = glfwGetWindowUserPointer(window);
    if (c->input.key_callback) {
        c->input.key_callback(c->owner, key, scancode, action, mods);
    }
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    update_input_mouse_cursor(xpos, ypos);

    callback_controller* c = glfwGetWindowUserPointer(window);
    if (c->input.cursor_pos_callback) {
        c->input.cursor_pos_callback(c->owner, xpos, ypos);
    }
}

void mouse_button_callback(GLFWwindow* window, i32 button, i32 action, i32 mods) {
    update_input_mouse_button(button, action, mods);

    callback_controller* c = glfwGetWindowUserPointer(window);
    if (c->input.mouse_button_callback) {
        c->input.mouse_button_callback(c->owner, button, action, mods);
    }
}

void sound_test(const char* path, f32 pitch, f32 gain) {
    audio_context audio;
    init_audio(&audio);

    f32 listenerOri[] = { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
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
    vec3 bottom_left = { -tran->scale[0] * 0.5, -tran->scale[1] * 0.5, 0 };
    vec3 top_right = { tran->scale[0] * 0.5, tran->scale[1] * 0.5, 0 };
    vec3 bottom_right = { tran->scale[0] * 0.5, -tran->scale[1] * 0.5, 0 };
    vec3 top_left = { -tran->scale[0] * 0.5, tran->scale[1] * 0.5, 0 };

    mat4 m4;
    mat3 m;
    glm_euler(tran->euler_angle, m4);
    glm_mat4_pick3(m4, m);
    glm_mat3_mulv(m, bottom_left, bottom_left);
    glm_mat3_mulv(m, bottom_right, bottom_right);
    glm_mat3_mulv(m, top_left, top_left);
    glm_mat3_mulv(m, top_right, top_right);

    glm_vec3_add(tran->position, bottom_left, bottom_left);
    glm_vec3_add(tran->position, bottom_right, bottom_right);
    glm_vec3_add(tran->position, top_left, top_left);
    glm_vec3_add(tran->position, top_right, top_right);

    DRAW_DEBUG_LINE(top_right, top_left, color);
    DRAW_DEBUG_LINE(top_left, bottom_left, color);
    DRAW_DEBUG_LINE(bottom_left, bottom_right, color);
    DRAW_DEBUG_LINE(bottom_right, top_right, color);

    vec3 red = {1, 0, 0};

    vec3 direction, right = {tran->right[0] * 0.5, tran->right[1] * 0.5, tran->right[2]};
    glm_vec3_add(tran->position, right, direction);
    DRAW_DEBUG_LINE(tran->position, direction, red);
}

void sprite_index_anim(anim_position_slide* slide, f32 dur) {
    const i32 key_frames = 2;
    i32 index = dur * key_frames;
    slide->target[0] = index % 2;
    slide->target[1] = 0;
}

typedef struct {
    transform tran;
    circle2d context;
    rigid2d body;
    collider2d collider;
} rigid2d_circle;

void rigid2d_circle_on_start(game_object* obj) {
    rigid2d_circle* self = obj->self;
    init_transform(&self->tran);
    init_rigid2d(&self->body, &self->tran);
    self->context = (circle2d){ .center = {0, 0}, .radius = 0.5 };
    self->collider = create_collider2d(ColliderCircle2d, &self->body, &self->context);
    self->body.collider = &self->collider;
    create_physics2d_object(&self->body);
}

void rigid2d_circle_on_update(game_object* obj) {
    rigid2d_circle* self = obj->self;
    vec3 a = {0, 0, 0};
    vec3 color = {1, 1, 1};
    glm_vec2_rotate((vec2){self->context.radius, 0}, self->tran.euler_angle[2], a);
    glm_vec2_add(a, self->tran.position, a);
    DRAW_DEBUG_LINE(self->tran.position, a, color);
    DRAW_DEBUG_CIRCLE(self->tran.position, self->context.radius, color);
    if (fabs(self->tran.position[0]) > 5 && fabs(self->tran.position[1]) > 5) {
        destory_game_object(obj);
    }
}

void rigid2d_circle_on_destory(game_object* obj) {
    rigid2d_circle* self = obj->self;
    destory_physics2d_object(&self->body);
}

typedef struct {
    transform tran;
    box2d context;
    rigid2d body;
    collider2d collider;
} rigid2d_box;

void rigid2d_box_on_start(game_object* obj) {
    rigid2d_box* self = obj->self;
    init_transform(&self->tran);
    init_rigid2d(&self->body, &self->tran);
    self->context = (box2d){ .center = {0, 0}, .size = {0.5, 0.5} };
    self->collider = create_collider2d(ColliderBox2d, &self->body, &self->context);
    self->body.collider = &self->collider;
    create_physics2d_object(&self->body);
}

void rigid2d_box_on_update(game_object* obj) {
    rigid2d_box* self = obj->self;
    render_transform_outline(&self->tran, (vec3){1, 1, 1});
    if (fabs(self->tran.position[0]) > 5 && fabs(self->tran.position[1]) > 5) {
        destory_game_object(obj);
    }
}

void rigid2d_box_on_destory(game_object* obj) {
    rigid2d_box* self = obj->self;
    destory_physics2d_object(&self->body);
}

typedef struct {
    transform tran;
    capsule2d context;
    rigid2d body;
    collider2d collider;
} rigid2d_capsule;

void rigid2d_capsule_on_start(game_object* obj) {
    rigid2d_capsule* self = obj->self;
    init_transform(&self->tran);
    init_rigid2d(&self->body, &self->tran);
    self->context = (capsule2d){ .center = {0, 0}, .height = 1, .radius = 0.5, .horizontal = 0 };
    self->collider = create_collider2d(ColliderCapsule2d, &self->body, &self->context);
    self->body.collider = &self->collider;
    create_physics2d_object(&self->body);
}

void rigid2d_capsule_on_update(game_object* obj) {
    rigid2d_capsule* self = obj->self;

    render_transform_outline(&self->tran, (vec3){1, 1, 1});

    capsule2d* cap = &self->context;
    i32 is_hori = cap->horizontal;
    vec3 cap_p1 = {-cap->height * 0.5 * is_hori, -cap->height * 0.5 * (1 - is_hori)};
    vec3 cap_p2 = {cap->height * 0.5 * is_hori, cap->height * 0.5 * (1 - is_hori)};

    glm_vec2_rotate(cap_p1, self->tran.euler_angle[2], cap_p1);
    glm_vec2_rotate(cap_p2, self->tran.euler_angle[2], cap_p2);
    glm_vec2_add(cap_p1, self->tran.position, cap_p1);
    glm_vec2_add(cap_p2, self->tran.position, cap_p2);

    draw_debug_circle(cap_p1, cap->radius, (vec3){1, 1, 1});
    draw_debug_circle(cap_p2, cap->radius, (vec3){1, 1, 1});

    if (fabs(self->tran.position[0]) > 5 && fabs(self->tran.position[1]) > 5) {
        destory_game_object(obj);
    }
}

void rigid2d_capsule_on_destory(game_object* obj) {
    rigid2d_box* self = obj->self;
    destory_physics2d_object(&self->body);
}

#define BOX_COUNT 2
#define MAX_POOL_OBJ 100
typedef struct {
    rigid2d_box ground[3];
    rigid2d_box box_pool[MAX_POOL_OBJ];
    rigid2d_circle circle_pool[MAX_POOL_OBJ];
    i32 pool_box_count, pool_circle_count;
} rigid2d_test;

void rigid2d_test_on_start(game_object* obj) {
    rigid2d_test* self = obj->self;
    self->pool_box_count = self->pool_circle_count = 0;

    f32 config[] = {
        // pos      size        res     angle
         0,   -2,   10, 0.25,   1,    0,
        -5,    1,   6, 0.25,   1,    90,
         5,    1,   6, 0.25,   1,    90,
        // -3.5, -2,   3,  0.25,   0.5,    150,
         // 3.5, -2,   3,  0.25,   0.5,   -150,
    };

    f32* cs = config;
    for (int i = 0; i < 3; i++, cs += 6) {
        create_game_object(&(game_object){
            .self = self->ground + i,
            .on_start = rigid2d_box_on_start,
            .on_activate = NULL,
            .on_update = rigid2d_box_on_update,
            .on_destory = rigid2d_box_on_destory,
        });
        glm_vec2_copy(cs + 0, self->ground[i].tran.position);
        glm_vec2_copy(cs + 2, self->ground[i].tran.scale);
        glm_vec2_copy(self->ground[i].tran.scale, self->ground[i].context.size);
        self->ground[i].body.restitution = cs[4];
        self->ground[i].tran.euler_angle[2] = glm_rad(cs[5]);
        self->ground[i].context.size[0] *= 0.5;
        self->ground[i].context.size[1] *= 0.5;
        rigid2d_set_static(&self->ground[i].body);
    }
}

void rigid2d_test_on_update(game_object* obj) {
    rigid2d_test* self = obj->self;
    camera* cam = find_game_object_by_index(0)->self;

    vec2 cursor;
    input_mouse_cursor(cursor);

    vec4 uv = { cursor[0] / cam->resolution[0] * 2 - 1, (1 - cursor[1] / cam->resolution[1]) * 2 - 1, 0, 1};
    mat4 m;
    glm_mat4_mul(cam->projection, cam->view, m);
    glm_mat4_inv(m, m);
    glm_mat4_mulv(m, uv, uv);
    
    static i32 down = 0;
    if (down == 0 && self->pool_box_count < MAX_POOL_OBJ && input_mouse_button_down(GLFW_MOUSE_BUTTON_LEFT)) {
        rigid2d_box* box = self->box_pool + self->pool_box_count;
        create_game_object(&(game_object){
            .self = box,
            .on_start = rigid2d_box_on_start,
            .on_activate = NULL,
            .on_update = rigid2d_box_on_update,
            .on_destory = rigid2d_box_on_destory,
        });
        glm_vec2_copy(uv, box->tran.position);
        box->body.restitution = 0.5;
        self->pool_box_count++;
        rigid2d_set_mass(&box->body, 0.5);
        down = 1;
    }
    if (down == 0 && self->pool_circle_count < MAX_POOL_OBJ && input_mouse_button_down(GLFW_MOUSE_BUTTON_RIGHT)) {
        rigid2d_circle* circle = self->circle_pool + self->pool_circle_count;
        create_game_object(&(game_object){
            .self = circle,
            .on_start = rigid2d_circle_on_start,
            .on_activate = NULL,
            .on_update = rigid2d_circle_on_update,
            .on_destory = rigid2d_circle_on_destory,
        });
        circle->context.radius = 0.3;
        circle->body.restitution = 0.5;
        glm_vec2_copy(uv, circle->tran.position);
        self->pool_circle_count++;
        rigid2d_set_mass(&circle->body, 0.5);
        down = 1;
    }
    if (input_mouse_button_release(GLFW_MOUSE_BUTTON_LEFT) && input_mouse_button_release(GLFW_MOUSE_BUTTON_RIGHT)) {
        down = 0;
    }

    f32 step = 1.0 / 144 * 4;
    if (input_key_press(GLFW_KEY_UP)) {
        translate_camera(cam, (vec3){0, step, 0});
    }
    else if (input_key_press(GLFW_KEY_DOWN)) {
        translate_camera(cam, (vec3){0, -step, 0});
    }
    if (input_key_press(GLFW_KEY_LEFT)) {
        translate_camera(cam, (vec3){-step, 0, 0});
    }
    else if (input_key_press(GLFW_KEY_RIGHT)) {
        translate_camera(cam, (vec3){step, 0, 0});
    }
}

void rigid2d_test_on_destory(game_object* obj) {
    rigid2d_test* self = obj->self;

    for (i32 i = 0; i < self->pool_box_count; ++i) {
        destory_physics2d_object(&self->box_pool[i].body);
    }
    for (i32 i = 0; i < self->pool_circle_count; ++i) {
        destory_physics2d_object(&self->circle_pool[i].body);
    }
    // destory_physics2d_object(&self->ground.body);
}

i32 main(void)
{
    trace_info ti = { .file_name = "tracing-init.json", };
    setup_trace_info(&ti);

    BEGIN_SCOPE_SESSION();
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    stbi_set_flip_vertically_on_load(1);
    END_SCOPE_SESSION(ti, "glfw init");

    GLFWwindow* app_window = NULL;
    BEGIN_SCOPE_SESSION();
    app_window = glfwCreateWindow(WIDTH, HEIGHT, "chess", NULL, NULL);
    END_SCOPE_SESSION(ti, "create window");

    glfwSwapInterval(1);

    glfwSetKeyCallback(app_window, key_callback);
    glfwSetFramebufferSizeCallback(app_window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(app_window, mouse_button_callback);
    glfwSetCursorPosCallback(app_window, cursor_pos_callback);
    
    BEGIN_SCOPE_SESSION();
    glfwMakeContextCurrent(app_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    END_SCOPE_SESSION(ti, "glfw make context");

    printf("Opengl Version %s\n", glGetString(GL_VERSION));

    BEGIN_SCOPE_SESSION();
    GLC(glEnable(GL_DEPTH_TEST));
    GLC(glEnable(GL_BLEND));
    GLC(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLC(glDepthFunc(GL_LESS));
    END_SCOPE_SESSION(ti, "enable gl stuffs");

    audio_context audio;

    BEGIN_SCOPE_SESSION();
    init_audio(&audio);
    END_SCOPE_SESSION(ti, "init audio");

    set_audio_listener_properties((vec3){0, 0, 0}, (vec3){0, 0, 0}, (f32[]){ 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f });

    // setting up
    BEGIN_SCOPE_SESSION();
    INIT_DEBUG_LINE_RENDERER();
    setup_input_system(app_window);
    init_sprite_instance();
    setup_game_object_system();
    setup_physics2d_object_system();
    setup_anim_system();
    END_SCOPE_SESSION(ti, "setup game");

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

    translate_camera(&cam, (vec3){0, 1.5, 0});
    cam.ortho.depth[0] = -10;
    cam.ortho.depth[1] = 10;
    cam.ortho.size = 5;
    set_camera_ortho_mat4(&cam);

    game.win_state.width = WIDTH;
    game.win_state.height = HEIGHT;
    
    glClearColor(0.0, 0.1, 0.1, 0);

    create_game_object(&(game_object){
        .self = &cam,
        .on_start = NULL,
        .on_activate = NULL,
        .on_update = NULL,
        .on_destory = NULL,
    });

    rigid2d_test test;
    create_game_object(&(game_object){
        .self = &test,
        .on_start = rigid2d_test_on_start,
        .on_activate = NULL,
        .on_update = rigid2d_test_on_update,
        .on_destory = rigid2d_test_on_destory,
    });

    rigid2d_capsule capsule_obj;
    create_game_object(&(game_object){
        .self = &capsule_obj,
        .on_start = rigid2d_capsule_on_start,
        .on_activate = NULL,
        .on_update = rigid2d_capsule_on_update,
        .on_destory = rigid2d_capsule_on_destory,
    });
    capsule_obj.body.restitution = 0.5;

    f32 pitch = 1, gain = 1;
    u32 buffers[2];
    u32 sources[2];

    {
        BEGIN_SCOPE_SESSION();
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
        END_SCOPE_SESSION(ti, "load minecraft1.mp3");
    }
    {
        BEGIN_SCOPE_SESSION();
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
        END_SCOPE_SESSION(ti, "load yippee.mp3");
    }

    sprite_texture sp_tex;
    BEGIN_SCOPE_SESSION();
    init_texture(&sp_tex.tex, "assets/Sprout-Lands/Characters/Basic-Charakter.png", TextureFilterNearest);
    glm_vec2_copy((vec2){48.0 / sp_tex.tex.width, 48.0 / sp_tex.tex.height}, sp_tex.per_sprite);
    END_SCOPE_SESSION(ti, "load tex Basic-Charakter.png");

    sprite sp = {
        .sprite_index = {0, 0},
        .color = {1, 1, 1, 1}
    };
    transform tran = {
        .position = {0, 0, 0},
        .scale = {2, 2, 1},
        .parent = NULL,
        .euler_angle = {0, 0, 0}
    };

    anim_position_slide sprite_anim;
    init_anim_position_slide(&sprite_anim, (vec3){4, 0, 0}, sprite_index_anim);
    set_anim_position_slide(&sprite_anim, sp.sprite_index);
    anim_duration anim = { .loop = 1 };
    init_anim_position_slide_duration(&anim, &sprite_anim, 0.5);
    create_anim_duration(&anim);

    while(!glfwWindowShouldClose(app_window))
    {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		update_anim_system();
		update_physics2d_object_system();
        update_game_object_system();
		con.window.render_callback(con.owner);

        render_sprite(&cam, &tran, &sp_tex, &sp);
        render_transform_outline(&tran, (vec3){1, 1, 1});

        glfwSwapBuffers(app_window);
        glfwPollEvents();
    }

    shutdown_game_object_system();
    shutdown_physics2d_object_system();
    shutdown_anim_system();
    shutdown_input_system();

    SHUTDOWN_DEBUG_LINE_RENDERER();

    glDeleteProgram(sprite_instance.shader);

    alDeleteBuffers(1, &sources[0]);
    alDeleteBuffers(1, &sources[1]);
    alDeleteBuffers(1, &buffers[0]);
    alDeleteBuffers(1, &buffers[1]);
    shutdown_audio(&audio);

    glfwDestroyWindow(app_window);

    glfwTerminate();
    CHECK_MEMORY_LEAK();

    end_tracing(&ti);

    return 0;
}
