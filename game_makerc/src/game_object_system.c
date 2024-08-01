#include "game_object_system.h"
#include "basic/vector.h"

static struct game_object_system {
    vector(game_object) objects;
} instance;

game_object_system* get_game_object_system() {
    return &instance;
}

void setup_game_object_system() {
    get_game_object_system()->objects = make_vector();
}

void create_game_object(game_object* obj) {
    ASSERT(obj != NULL);
    vector_pushe(instance.objects, *obj);
    if (obj->on_start) {
        obj->on_start(obj);
    }
}

void update_game_object_system() {
    for_vector(instance.objects, i, 0) {
        game_object* obj = &instance.objects[i];
        if (obj->on_update) {
            obj->on_update(obj);
        }
    }
}

game_object* find_game_object_by_index(int index) {
    ASSERT(index >= 0);
    if (index < (int)vector_size(instance.objects)) {
        return &instance.objects[index];
    }
    return NULL;
}

void destory_game_object(game_object* obj) {
    ASSERT(obj != NULL);
    for_vector(instance.objects, i, 0) {
        if (&instance.objects[i] == obj) {
        	if (obj->on_destory) {
	            obj->on_destory(obj);
        	}
            instance.objects[i] = vector_back(instance.objects);
            vector_pop(instance.objects);
            break;
        }
    }
}

void shutdown_game_object_system() {
    for_vector(instance.objects, i, 0) {
    	if (instance.objects[i].on_destory) {
			instance.objects[i].on_destory(instance.objects + i);
    	}
    }
    free_vector(instance.objects);
}