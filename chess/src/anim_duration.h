#ifndef _ANIM_DURATION_
#define _ANIM_DURATION_
#include "util.h"

typedef struct anim_duration anim_duration;
typedef void(*anim_duration_callback)(anim_duration*, float);
struct anim_duration {
    anim_duration_callback callback[2];
    f32 time_start, time_duration;
    void* in;
    i32 ended, index;
};

void init_anim_duration(anim_duration* anim, void* in, float time_duration, anim_duration_callback callback);
void anim_duration_end_callback(anim_duration* in, float);
void activate_anim_duration(anim_duration* anim);
void anim_duration_start(float time, anim_duration* anim);

#endif
