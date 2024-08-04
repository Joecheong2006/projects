#include "anim_duration.h"
#include "anim_duration_system.h"
#include "platform/platform.h"
#include "core/assert.h"

void anim_duration_end_callback(anim_duration* in, f32 dur) {
	ASSERT_MSG(in, "invalid anim_duration");
	(void)dur;
    anim_duration* anim = in;
    if (!anim->loop) {
	    anim->ended = 1;
    }
    anim->time_start = platform_get_time();
}

void init_anim_duration(anim_duration* anim, void* in, f32 time_duration, anim_duration_callback callback) {
	ASSERT(anim != NULL && in != NULL);
    anim->callback = callback;
    anim->in = in;
    anim->time_start = -1;
    anim->time_duration = time_duration;
    anim->ended = 0;
    anim->index = -1;
}

void activate_anim_duration(anim_duration* anim) {
	ASSERT_MSG(anim != NULL, "invalid anim param");
    anim->time_start = platform_get_time();
    anim->index = get_anim_duration_num();
}

void anim_duration_start(f32 time, anim_duration* anim) {
	ASSERT_MSG(anim != NULL, "invalid anim param");
	if (anim->ended) {
	    return;
	}
	f32 dur = time - anim->time_start;
    if (dur / anim->time_duration <= 1) {
	    anim->callback(anim, dur);
	    return;
    }
    anim_duration_end_callback(anim, dur);
}
