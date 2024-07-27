#ifndef _GAME_OBJECT_
#define _GAME_OBJECT_

typedef struct game_object game_object;
typedef void(*game_object_start_callback)(game_object*);
typedef void(*game_object_activate_callback)(game_object*);
typedef void(*game_object_update_callback)(game_object*);
typedef void(*game_object_render_callback)(game_object*);
typedef void(*game_object_destory_callback)(game_object*);

struct game_object {
    void* self;
    game_object_start_callback on_start;
    game_object_activate_callback on_activate;
    game_object_update_callback on_update;
    game_object_render_callback on_render;
    game_object_destory_callback on_destory;
};

#endif
