#ifndef _GAME_OBJECT_SYSTEM_
#define _GAME_OBJECT_SYSTEM_
#include "game_object.h"

typedef struct game_object_system game_object_system;

game_object_system* get_game_object_system();
void init_game_object_system();
void create_game_object(game_object* obj);
void update_game_object_system();
game_object* find_game_object_by_index(int index);
void destory_game_object(game_object* obj);
void shutdown_game_object_system();

#endif
