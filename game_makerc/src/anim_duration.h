#ifndef _ANIM_DURATION_H_
#define _ANIM_DURATION_H_
#include "core/defines.h"

typedef struct anim_duration anim_duration;
typedef void(*anim_duration_callback)(anim_duration*, f32);
struct anim_duration {
    anim_duration_callback callback;
    f32 time_start, time_duration;
    void* in;
    i32 ended, index, loop;
};

void init_anim_duration(anim_duration* anim, void* in, f32 time_duration, anim_duration_callback callback);
void anim_duration_end_callback(anim_duration* in, f32);
void activate_anim_duration(anim_duration* anim);
void anim_duration_start(f32 time, anim_duration* anim);

#endif
