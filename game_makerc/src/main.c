#include <glad/glad.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cglm/cglm.h>
#include <string.h>

#include "core/log.h"
#include "platform/platform.h"

#include "camera.h"
#include "cglm/vec2.h"
#include "string.h"
#include "opengl/opengl_buffer_object.h"
#include "sprite.h"
#include "stb_image.h"

#include "window_callback.h"

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

#define PERSPECTIVE_CAMERA

#define PI 3.14159265359

#define WIDTH 640
#define HEIGHT 640

typedef struct {
    GLFWwindow* window;
    i32 width, height;
} window_state;

void close_application(window_state* window) {
    glfwSetWindowShouldClose(window->window, GLFW_TRUE);
}

typedef struct {
    sprite_texture sp_tex;
    sprite sp;
    transform tran;
    anim_position_slide sprite_anim;
} sprite_obj;

typedef struct {
    window_state win_state;
    platform_state plat_state;

    audio_context audio;
    camera cam;
    sprite_obj sp_obj;

    u32 buffers[2];
    u32 sources[2];

    window_callback_setup con;
} Game;

void game_window_resize_callback(void* owner, i32 width, i32 height) {
    Game* game = owner;

    camera* cam = find_game_object_by_index(0)->self;
    cam->resolution[0] = width;
    cam->resolution[1] = height;

    game->win_state.width = width;
    game->win_state.height = height;
    
#if defined(PERSPECTIVE_CAMERA)
    cam->persp.aspect = cam->resolution[0] / cam->resolution[1];
    set_camera_persp_mat4(cam);
#else
    set_camera_ortho_mat4(cam);
#endif

    glViewport(0, 0, width, height);
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

void sound_test(const char* path, f32 pitch, f32 gain) {
    audio_context audio;
    init_audio(&audio);

    f32 listenerOri[] = { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
    set_audio_listener_properties((vec3){0, 0, 0}, (vec3){0, 0, 0}, listenerOri);

    u32 source = create_audio_source(pitch, gain, (vec3){0, 0, 0}, (vec3){0, 0, 0}, 0);

    u32 buffer = gen_sound_buffer(path);
    if (!buffer) {
        printf("load %s failed\n", path);
        release_audio_source(source);
        shutdown_audio(&audio);
        return;
    }

    set_audio_source_buffer(source, buffer);
    al_check_error();

    audio_play(source);
    al_check_error();

    AudioSourceState state = get_audio_source_state(source);
    al_check_error();
    while (state == AudioSourcePlaying) {
        state = get_audio_source_state(source);
        al_check_error();
    }

    release_audio_source(source);
    release_audio_source_buffer(buffer);
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

    render_debug_line(top_right, top_left, color);
    render_debug_line(top_left, bottom_left, color);
    render_debug_line(bottom_left, bottom_right, color);
    render_debug_line(bottom_right, top_right, color);

    vec3 red = {1, 0, 0};

    vec3 direction, right = {tran->right[0] * 0.5, tran->right[1] * 0.5, tran->right[2]};
    glm_vec3_add(tran->position, right, direction);
    render_debug_line(tran->position, direction, red);
}

void sprite_index_anim(anim_position_slide* slide, f32 dur) {
    const i32 key_frames = 4;
    i32 index = dur * key_frames;
    slide->target[0] = index % 2 + 2;
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
    render_debug_line(self->tran.position, a, color);
    draw_debug_circle(self->tran.position, self->context.radius, color);
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

    draw_debug_circle(uv, 0.1, (vec3){0, 1, 1});
    
    static i32 down = 0;
    if (down == 0 && self->pool_box_count < MAX_POOL_OBJ && input_mouse_button_down(GLFW_MOUSE_BUTTON_LEFT)) {
        rigid2d_box* box = self->box_pool + self->pool_box_count;
        create_game_object(&(game_object){
            .self = box,
            .on_start = rigid2d_box_on_start,
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

void camera_controller_start(game_object* obj) {
    (void)obj;
    camera* cam = find_game_object_by_index(0)->self;
#if defined(PERSPECTIVE_CAMERA)
    cam->persp = (camera_persp_state){
        .fov = glm_rad(60),
        .aspect = cam->resolution[0] / cam->resolution[1],
        .near = 0.1,
        .far = 100
    };
    set_camera_persp_mat4(cam);
#else
    cam->ortho.depth[0] = -10;
    cam->ortho.depth[1] = 10;
    cam->ortho.size = 5;
    set_camera_ortho_mat4(cam);
#endif
}

void camera_controller_update(game_object* obj) {
    (void)obj;
    f32 step = 1.0 / 144 * 4;
    camera* cam = find_game_object_by_index(0)->self;
#if !defined PERSPECTIVE_CAMERA
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

#else
    step *= 0.7;
    vec3 dir;
    if (input_key_press(GLFW_KEY_W)) {
        glm_vec3_copy(cam->tran.forward, dir);
        translate_camera(cam, (vec3){dir[0] * step, dir[1] * step, dir[2] * step});
    }
    else if (input_key_press(GLFW_KEY_S)) {
        glm_vec3_copy(cam->tran.forward, dir);
        translate_camera(cam, (vec3){dir[0] * -step, dir[1] * -step, dir[2] * -step});
    }
    if (input_key_press(GLFW_KEY_A)) {
        glm_vec3_copy(cam->tran.right, dir);
        translate_camera(cam, (vec3){dir[0] * -step, dir[1] * -step, dir[2] * -step});
    }
    else if (input_key_press(GLFW_KEY_D)) {
        glm_vec3_copy(cam->tran.right, dir);
        translate_camera(cam, (vec3){dir[0] * step, dir[1] * step, dir[2] * step});
    }
    if (input_key_press(GLFW_KEY_SPACE)) {
        glm_vec3_copy(cam->tran.up, dir);
        translate_camera(cam, (vec3){dir[0] * step, dir[1] * step, dir[2] * step});
    }

    static vec2 last_pos = {WIDTH / 2.0, HEIGHT / 2.0};

    vec2 current_pos, offset;
    input_mouse_cursor(current_pos);

    if (input_mouse_button_down(GLFW_MOUSE_BUTTON_LEFT)) {
        glm_vec2_copy(current_pos, last_pos);
    }

    glm_vec2_sub(current_pos, last_pos, offset);
    cam->tran.euler_angle[1] -= offset[1] * 0.04;
    cam->tran.euler_angle[2] += offset[0] * 0.04;
    glm_vec2_copy(current_pos, last_pos);

    vec3 direction;
    direction[0] = cos(glm_rad(cam->tran.euler_angle[2])) * cos(glm_rad(cam->tran.euler_angle[1]));
    direction[1] = sin(glm_rad(cam->tran.euler_angle[1]));
    direction[2] = sin(glm_rad(cam->tran.euler_angle[2])) * cos(glm_rad(cam->tran.euler_angle[1]));
    glm_vec3_normalize_to(direction, cam->tran.forward);
    vec3 center;
    glm_vec3_add(cam->tran.forward, cam->tran.position, center);
    glm_lookat(cam->tran.position, center, cam->tran.up, cam->view);
    glm_vec3_cross(cam->tran.forward, cam->tran.up, cam->tran.right);
    set_camera_persp_mat4(cam);
#endif
}

static rigid2d_test test;
i32 on_initialize(void* self) {
    Game* game = self;
    setup_platform(&game->plat_state);

    LOG_INFO("%s\n", "hello world!");
    LOG_WARN("%s\n", "hello world!");
    LOG_DEBUG("%s\n", "hello world!");
    LOG_TRACE("%s\n", "hello world!");
    LOG_ERROR("%s\n", "hello world!");
    LOG_FATAL("%s\n", "hello world!");
    
    trace_info ti = { .file_name = "tracing-init.json", };
    setup_trace_info(&ti);

    BEGIN_SCOPE_SESSION();
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    END_SCOPE_SESSION(ti, "glfw init");
    stbi_set_flip_vertically_on_load(1);

    BEGIN_SCOPE_SESSION();
    platform_sleep(13);
    END_SCOPE_SESSION(ti, "testing sleep for 13ms");

    BEGIN_SCOPE_SESSION();
    game->win_state.window = glfwCreateWindow(WIDTH, HEIGHT, "chess", NULL, NULL);
    game->win_state.height = HEIGHT;
    game->win_state.width = WIDTH;
    END_SCOPE_SESSION(ti, "create window");

    glfwSwapInterval(1);
    
    BEGIN_SCOPE_SESSION();
    glfwMakeContextCurrent(game->win_state.window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    END_SCOPE_SESSION(ti, "glfw make context");

    glfwSetInputMode(game->win_state.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    printf("Opengl Version %s\n", glGetString(GL_VERSION));

    BEGIN_SCOPE_SESSION();
    GLC(glEnable(GL_DEPTH_TEST));
    GLC(glEnable(GL_BLEND));
    GLC(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLC(glDepthFunc(GL_LESS));
    END_SCOPE_SESSION(ti, "enable gl stuffs");

    BEGIN_SCOPE_SESSION();
    init_audio(&game->audio);
    END_SCOPE_SESSION(ti, "init audio");

    set_audio_listener_properties((vec3){0, 0, 0}, (vec3){0, 0, 0}, (f32[]){ 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f });

    // setting up
    BEGIN_SCOPE_SESSION();
    init_debug_line_renderer();
    setup_input_system(game->win_state.window);
    init_sprite_instance();
    setup_game_object_system();
    setup_physics2d_object_system();
    setup_anim_system();
    END_SCOPE_SESSION(ti, "setup game");

    game->con = (window_callback_setup) {
        .owner = game,
        .input.on_cursor_pos = NULL,
        .input.on_key = game_key_callback,
        .input.on_mouse_button = NULL,
        .window.on_resize = game_window_resize_callback,
        .window.on_render = NULL,
    };
    setup_window_callback(game->win_state.window, &game->con);

    init_camera(&game->cam, (vec2){WIDTH, HEIGHT});
    translate_camera(&game->cam, (vec3){0, 1.5, 0});

    game->win_state.width = WIDTH;
    game->win_state.height = HEIGHT;
    
    glClearColor(0.0, 0.1, 0.1, 0);

    create_game_object(&(game_object){
        .self = &game->cam,
        .on_start = NULL,
        .on_update = NULL,
        .on_destory = NULL,
    });

    create_game_object(&(game_object){
        .self = NULL,
        .on_start = camera_controller_start,
        .on_update = camera_controller_update,
        .on_destory = NULL,
    });

    create_game_object(&(game_object){
        .self = &test,
        .on_start = rigid2d_test_on_start,
        .on_update = rigid2d_test_on_update,
        .on_destory = rigid2d_test_on_destory,
    });

    f32 pitch = 1, gain = 1;

    {
        BEGIN_SCOPE_SESSION();
        game->sources[0] = create_audio_source(pitch, gain, (vec3){0, 0, 0}, (vec3){0, 0, 0}, 0);
        const char* audio_file_path = "assets/audio/minecraft1.mp3";
        game->buffers[0] = gen_sound_buffer(audio_file_path);
        if (!game->buffers[0]) {
            LOG_WARN("load %s failed\n", audio_file_path);
            release_audio_source(game->sources[0]);
            return 0;
        }

        set_audio_source_buffer(game->sources[0], game->buffers[0]);
        audio_play(game->sources[0]);
        END_SCOPE_SESSION(ti, "load minecraft1.mp3");
    }
    {
        BEGIN_SCOPE_SESSION();
        game->sources[1] = create_audio_source(pitch, gain, (vec3){0, 0, 0}, (vec3){0, 0, 0}, 0);
        const char* audio_file_path = "assets/audio/yippee.mp3";
        game->buffers[1] = gen_sound_buffer(audio_file_path);
        if (!game->buffers[1]) {
            LOG_WARN("load %s failed\n", audio_file_path);
            release_audio_source(game->sources[1]);
            return 0;
        }

        set_audio_source_buffer(game->sources[1], game->buffers[1]);
        audio_play(game->sources[1]);
        END_SCOPE_SESSION(ti, "load yippee.mp3");
    }

    BEGIN_SCOPE_SESSION();
    if (init_texture(&game->sp_obj.sp_tex.tex, "assets/Sprout-Lands/Characters/Basic-Charakter.png", TextureFilterNearest) != ErrorNone) {
        return 0;
    }
    glm_vec2_copy((vec2){48.0 / game->sp_obj.sp_tex.tex.width, 48.0 / game->sp_obj.sp_tex.tex.height}, game->sp_obj.sp_tex.per_sprite);
    END_SCOPE_SESSION(ti, "load tex Basic-Charakter.png");

    game->sp_obj.sp = (sprite) {
        .sprite_index = {0, 0},
        .color = {1, 1, 1, 1}
    };
    game->sp_obj.tran = (transform) {
        .position = {0, 0, 0},
        .scale = {2, 2, 1},
        .parent = NULL,
        .euler_angle = {0, 0, 0}
    };

    init_anim_position_slide(&game->sp_obj.sprite_anim, (vec3){4, 0, 0}, sprite_index_anim);
    set_anim_position_slide(&game->sp_obj.sprite_anim, game->sp_obj.sp.sprite_index);
    anim_duration anim = { .loop = 1 };
    init_anim_position_slide_duration(&anim, &game->sp_obj.sprite_anim, 0.5);
    create_anim_duration(&anim);

    end_tracing(&ti);

    return 1;
}

i32 is_running(void * self) {
    Game* game = self;
    return !glfwWindowShouldClose(game->win_state.window);
}

void on_update(void* self) {
    Game* game = self;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	update_anim_system();
	update_physics2d_object_system();
    update_game_object_system();
    if (game->con.window.on_render) {
		game->con.window.on_render(game->con.owner);
    }

    render_sprite(&game->cam, &game->sp_obj.tran, &game->sp_obj.sp_tex, &game->sp_obj.sp);
    render_transform_outline(&game->sp_obj.tran, (vec3){1, 1, 1});

    glfwSwapBuffers(game->win_state.window);
    glfwPollEvents();
}

void on_terminate(void* self) {
    Game* game = self;
    shutdown_game_object_system();
    shutdown_physics2d_object_system();
    shutdown_anim_system();
    shutdown_input_system();
    shutdown_debug_line_renderer();

    glDeleteProgram(sprite_instance.shader);

    release_audio_source(game->sources[0]);
    release_audio_source(game->sources[1]);
    release_audio_source_buffer(game->buffers[0]);
    release_audio_source_buffer(game->buffers[1]);
    shutdown_audio(&game->audio);

    glfwDestroyWindow(game->win_state.window);

    glfwTerminate();
    
    shutdown_platform(&game->plat_state);
}

#include "application_setup.h"
#include "entry_point.h"

#include "basic/vector.h"
#include "basic/string.h"

typedef struct {
    GLFWwindow* window;
    VkInstance instance;
} vulkan_test;

i32 enable_validation_layers(const char** layer_names, u32 layers_count) {
    u32 layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);
    VkLayerProperties layers[layer_count];
    vkEnumerateInstanceLayerProperties(&layer_count, layers);

    for (u32 i = 0; i < layers_count; i++) {
        i32 layer_founded = 0;
        for (u32 j = 0; j < layer_count; j++) {
            if (strcmp(layers[i].layerName, layer_names[i]) == 0) {
                layer_founded = 1;
                break;
            }
        }
        if (!layer_founded) {
            return 0;
        }
            
    }
    return 1;
}

const vector(char*) get_vulkan_required_instance_extensions() {
    const vector(char*) result = make_vector();

    u32 glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    for (u32 i = 0; i < glfw_extension_count; i++) {
        vector_push(result, make_string((const string)glfw_extensions[i]));
    }

    return result;
}

i32 vulkan_test_init(void* self) {
    vulkan_test* vt = self;
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    vt->window = glfwCreateWindow(WIDTH, HEIGHT, "vulkan test", NULL, NULL);

    VkApplicationInfo app_info;
    memset(&app_info, 0, sizeof(VkApplicationInfo));
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "test";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo create_info;
    memset(&create_info, 0, sizeof(VkInstanceCreateInfo));
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    const char* validation_layer_names[] = {
        "VK_LAYER_KHRONOS_validation",
    };
    const u32 validation_layer_count = sizeof(validation_layer_names) / sizeof(char*);
    i32 validation_layer_available = enable_validation_layers(validation_layer_names, validation_layer_count);

    const char** glfw_extensions = get_vulkan_required_instance_extensions();
    for (u32 i = 0; i < validation_layer_count; i++) {
        vector_push(glfw_extensions, make_string(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));
    }

    u32 glfw_extension_count = vector_size(glfw_extensions);
    
    create_info.enabledExtensionCount = glfw_extension_count;
    create_info.ppEnabledExtensionNames = glfw_extensions;

    if (validation_layer_available) {
        LOG_WARN("\t%s\n", "validation layers are not available");
    }
    else {
        LOG_TRACE("\t%s\n", "validation layers are available");
    }

    if (!validation_layer_available) {
        create_info.enabledLayerCount = 0;
    }
    else {
        create_info.enabledLayerCount = validation_layer_count;
        create_info.ppEnabledLayerNames = validation_layer_names;
    }

    LOG_INFO("\t%s\n", "----- glfw extensions -----");
    for (u32 i = 0; i < glfw_extension_count; i++) {
        LOG_TRACE("\t%s\n", glfw_extensions[i]);
    }

    LOG_INFO("\t%s\n", "----- vuklan extensions -----");
    u32 extension_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);

    VkExtensionProperties extensions[extension_count];
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extensions);

    for (u32 i = 0; i < extension_count; i++) {
        LOG_TRACE("\t%s\n", extensions[i].extensionName);
    }

    VkResult result = vkCreateInstance(&create_info, NULL, &vt->instance);
    if (result != VK_SUCCESS) {
        LOG_FATAL("%s with code %d\n", "failed to create vuklan instance", result);
        return 0;
    }
    LOG_INFO("\t%s\n", "create vuklan instance success");

    for (u32 i = 0; i < glfw_extension_count; i++) {
        free_string((const string)glfw_extensions[i]);
    }
    free_vector(glfw_extensions);

    i32 leak_count = check_memory_leak();
    LOG_TRACE("\tleak_count = %d\n", leak_count);
    return 1;
}

i32 vulkan_test_is_running(void* self) {
    vulkan_test* vt = self;
    return !glfwWindowShouldClose(vt->window);
}

void vulkan_test_update(void* self) {
    vulkan_test* vt = self;

    glfwPollEvents();
}

void vulkan_test_terminate(void* self) {
    vulkan_test* vt = self;

    vkDestroyInstance(vt->instance, NULL);
    glfwDestroyWindow(vt->window);
    glfwTerminate();
}

application_setup create_application() {
    {
        static vulkan_test test;
        application_setup setup = {
            .app = &test,
            .on_initialize = vulkan_test_init,
            .is_running = vulkan_test_is_running,
            .on_update = vulkan_test_update,
            .on_terminate = vulkan_test_terminate,
        };
        return setup;
    }

    static Game game;
    application_setup setup = {
        .app = &game,
        .on_initialize = on_initialize,
        .is_running = is_running,
        .on_update = on_update,
        .on_terminate = on_terminate,
    };
    return setup;
}
