#include "anim_duration.h"
#include "anim_duration_system.h"
#include <GLFW/glfw3.h>

void anim_duration_end_callback(anim_duration* in, f32 dur) {
	(void)dur;
    anim_duration* anim = in;
    if (!anim->loop) {
	    anim->ended = 1;
    }
    anim->time_start = glfwGetTime();
}

void init_anim_duration(anim_duration* anim, void* in, f32 time_duration, anim_duration_callback callback) {
	ASSERT(anim != NULL && in != NULL);
    anim->callback[0] = callback;
    anim->callback[1] = anim_duration_end_callback;
    anim->in = in;
    anim->time_start = -1;
    anim->time_duration = time_duration;
    anim->ended = 0;
    anim->index = -1;
}

void activate_anim_duration(anim_duration* anim) {
	ASSERT(anim != NULL);
    anim->time_start = glfwGetTime();
    anim->index = get_anim_duration_num();
}

void anim_duration_start(f32 time, anim_duration* anim) {
	ASSERT(anim != NULL);
	if (anim->ended) {
	    return;
	}
	f32 dur = time - anim->time_start;
    anim->callback[(i32)(dur / anim->time_duration)](anim, dur);
}
