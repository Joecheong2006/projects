#include "anim_duration_system.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "vector.h"
#include "util.h"

struct anim_duration_system {
    vector(anim_duration) durations;
};

anim_duration_system* get_anim_duration_system() {
    static anim_duration_system instance;
    return &instance;
}

void setup_anim_system() {
    get_anim_duration_system()->durations = make_vector();
}

void create_anim_duration(anim_duration* duration) {
    ASSERT(duration != NULL);
    activate_anim_duration(duration);
    vector_pushe(get_anim_duration_system()->durations, *duration);
}

static int get_last_empty_slot_index() {
    anim_duration_system* system = get_anim_duration_system();
    int last_empty_index = get_anim_duration_num() - 1;
    for (i64 i = last_empty_index; i > -1; --i) {
        if (!system->durations[i].ended) {
            return last_empty_index;
        }
    }
    return -1;
}

static void clean_anim_duration() {
    anim_duration_system* system = get_anim_duration_system();

    int last_empty_index = get_last_empty_slot_index();

    int len = get_anim_duration_num();
    for (int i = 0; i < len; ++i) {
        if (system->durations[i].ended) {
            delete_anim_duration(&system->durations[i]);
        }
    }

    for (int i = 0; i < last_empty_index; ++i) {
        if (system->durations[i].ended) {
            int index = system->durations[i].index;
            system->durations[i] = system->durations[last_empty_index--];
            system->durations[i].index = index;
        }
    }
    vector_size(system->durations) = last_empty_index + 1;
}

void update_anim_system() {
    anim_duration_system* system = get_anim_duration_system();
    float time = glfwGetTime();

    for_vector(system->durations, i, 0) {
        anim_duration_start(time, system->durations + i);
    }

    clean_anim_duration();
}

void delete_anim_duration(anim_duration* duration) {
    ASSERT(duration != NULL);
    anim_duration_system* system = get_anim_duration_system();
    anim_duration temp = system->durations[duration->index];
    system->durations[duration->index] = vector_back(system->durations);
    system->durations[duration->index].index = temp.index;
    vector_pop(system->durations);
    // printf("delete %d:%d\n", duration->index, system->durations[duration->index].index);
}

void shutdown_anim_system() {
    anim_duration_system* system = get_anim_duration_system();
    printf("%lld\n", vector_size(system->durations));
    free_vector(system->durations);
}

int get_anim_duration_num() {
    return vector_size(get_anim_duration_system()->durations);
}
