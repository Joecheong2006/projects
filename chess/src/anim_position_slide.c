#include "anim_position_slide.h"

void anim_position_duration_callback(anim_duration* anim, float dur) {
    anim_position_slide* slide = anim->in;
    slide->callback(slide, dur / anim->time_duration);
}

void init_anim_position_slide(anim_position_slide* slide, vec3 translation, anim_position_slide_callback callback) {
    slide->callback = callback;
    glm_vec3_copy(translation, slide->translation);
}

void set_anim_position_slide(anim_position_slide* src, anim_position_slide* dest, vec3* target_position) {
    dest->callback = src->callback;
    dest->target = target_position;
    glm_vec3_copy(*dest->target, dest->start);
    glm_vec3_copy(src->translation, dest->translation);
    dest->end[0] = dest->start[0] + dest->translation[0];
    dest->end[1] = dest->start[1] + dest->translation[1];
    dest->end[2] = dest->start[2] + dest->translation[2];
}

void init_anim_position_slide_duration(anim_duration* anim, anim_position_slide* slide, float time_duration) {
    init_anim_duration(anim, slide, time_duration, anim_position_duration_callback);
}