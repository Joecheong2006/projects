#ifndef _ANIM_POSITION_SLIDE_
#define _ANIM_POSITION_SLIDE_
#include <cglm/cglm.h>
#include "anim_duration.h"

typedef struct anim_position_slide anim_position_slide;
typedef void(*anim_position_slide_callback)(anim_position_slide*, float);
struct anim_position_slide {
    vec3 translation, start, end;
    vec3* target;
    anim_position_slide_callback callback;
};

void init_anim_position_slide(anim_position_slide* slide, vec3 translation, anim_position_slide_callback callback);
void set_anim_position_slide(anim_position_slide* slide, vec3* target_position);
void init_anim_position_slide_duration(anim_duration* anim, anim_position_slide* slide, float time_duration);

#endif
