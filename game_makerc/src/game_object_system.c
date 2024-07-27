#include "game_object_system.h"
#include "vector.h"

struct game_object_system {
    vector(game_object) objects;
};

game_object_system* get_game_object_system() {
    static game_object_system instance;
    return &instance;
}

void init_game_object_system() {
    get_game_object_system()->objects = make_vector();
}

void create_game_object(game_object* obj) {
    ASSERT(obj != NULL);
    game_object_system* system = get_game_object_system();
    vector_pushe(system->objects, *obj);
    if (obj->on_start) {
        obj->on_start(obj);
    }
}

void update_game_object_system() {
    game_object_system* system = get_game_object_system();
    for_vector(system->objects, i, 0) {
        game_object* obj = &system->objects[i];
        if (obj->on_update) {
            obj->on_update(obj);
        }
        if (obj->on_render) {
            obj->on_render(obj);
        }
    }
}

game_object* find_game_object_by_index(int index) {
    ASSERT(index >= 0);
    game_object_system* system = get_game_object_system();
    if (index < (int)vector_size(system->objects)) {
        return &system->objects[index];
    }
    return NULL;
}

void destory_game_object(game_object* obj) {
    ASSERT(obj != NULL);
    game_object_system* system = get_game_object_system();
    for_vector(system->objects, i, 0) {
        if (&system->objects[i] == obj) {
        	if (obj->on_destory) {
	            obj->on_destory(obj);
        	}
            system->objects[i] = vector_back(system->objects);
            vector_pop(system->objects);
            break;
        }
    }
}

void shutdown_game_object_system() {
    game_object_system* system = get_game_object_system();
    for_vector(system->objects, i, 0) {
    	if (system->objects[i].on_destory) {
			system->objects[i].on_destory(system->objects + i);
    	}
    }
    free_vector(system->objects);
}