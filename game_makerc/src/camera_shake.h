#ifndef _CAMERA_SHAKE_H_
#define _CAMERA_SHAKE_H_
#include <cglm/cglm.h>
#include "core/defines.h"

typedef struct {
    f32 duration, strength;
    f32 time_start;
    vec3 origin_pos;
} camera_shake_object;

void create_camera_shake(camera_shake_object* obj);

#endif
