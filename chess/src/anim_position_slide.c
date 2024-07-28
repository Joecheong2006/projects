#include "anim_position_slide.h"

static void anim_position_duration_callback(anim_duration* anim, float dur) {
    anim_position_slide* slide = anim->in;
    slide->callback(slide, dur / anim->time_duration);
}

void init_anim_position_slide(anim_position_slide* slide, vec3 translation, anim_position_slide_callback callback) {
    slide->callback = callback;
    glm_vec3_copy(translation, slide->translation);
}

void set_anim_position_slide(anim_position_slide* slide, vec3* target_position) {
    slide->target = target_position;
    glm_vec3_copy(*slide->target, slide->start);
    slide->end[0] = slide->start[0] + slide->translation[0];
    slide->end[1] = slide->start[1] + slide->translation[1];
    slide->end[2] = slide->start[2] + slide->translation[2];
}

void init_anim_position_slide_duration(anim_duration* anim, anim_position_slide* slide, float time_duration) {
    init_anim_duration(anim, slide, time_duration, anim_position_duration_callback);
}