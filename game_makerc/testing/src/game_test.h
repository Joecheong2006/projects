#ifndef _GAME_TEST_H_
#define _GAME_TEST_H_

#include "core/defines.h"
#include "platform/platform.h"
#include "anim_position_slide.h"
#include "sprite.h"
#include "audio.h"

#include "window_callback.h"

typedef struct GLFWwindow GLFWwindow;
typedef struct {
    GLFWwindow* window;
    i32 width, height;
} window_state;

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

i32 game_test_on_initialize(void* self);
i32 game_test_is_running(void * self);
void game_test_on_update(void* self);
void game_test_on_terminate(void* self);

#endif
