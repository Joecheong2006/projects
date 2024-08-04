#include "anim_duration_system.h"
#include "core/assert.h"
#include "platform/platform.h"
#include <stdio.h>
#include "basic/vector.h"

static struct anim_duration_system {
    vector(anim_duration) durations;
} instance;

anim_duration_system* get_anim_duration_system() {
    return &instance;
}

void setup_anim_system() {
    instance.durations = make_vector();
}

void create_anim_duration(anim_duration* duration) {
    ASSERT(duration != NULL);
    activate_anim_duration(duration);
    vector_pushe(instance.durations, *duration);
}

static int get_last_empty_slot_index() {
    int last_empty_index = get_anim_duration_num() - 1;
    for (i64 i = last_empty_index; i > -1; --i) {
        if (!instance.durations[i].ended) {
            return last_empty_index;
        }
    }
    return -1;
}

static void clean_anim_duration() {
    int last_empty_index = get_last_empty_slot_index();

    int len = get_anim_duration_num();
    for (int i = 0; i < len; ++i) {
        if (instance.durations[i].ended) {
            delete_anim_duration(instance.durations + i);
        }
    }

    for (int i = 0; i < last_empty_index; ++i) {
        if (instance.durations[i].ended) {
            int index = instance.durations[i].index;
            instance.durations[i] = instance.durations[last_empty_index--];
            instance.durations[i].index = index;
        }
    }
    vector_size(instance.durations) = last_empty_index + 1;
}

void update_anim_system() {
    f32 time = platform_get_time();

    i32 len = vector_size(instance.durations);
    for (i32 i = 0; i < len; i++) {
        anim_duration_start(time, instance.durations + i);
    }

    clean_anim_duration();
}

void delete_anim_duration(anim_duration* duration) {
    anim_duration temp = instance.durations[duration->index];
    instance.durations[duration->index] = vector_back(instance.durations);
    instance.durations[duration->index].index = temp.index;
    vector_pop(instance.durations);
}

void shutdown_anim_system() {
    LOG_TRACE("%lld\n", vector_size(instance.durations));
    free_vector(instance.durations);
}

int get_anim_duration_num() {
    return vector_size(instance.durations);
}
