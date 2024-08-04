#ifndef _CAMERA2D_H_
#define _CAMERA2D_H_
#include <cglm/cglm.h>
#include "core/defines.h"
#include "transform.h"

typedef struct {
    vec2 depth;
    f32 size;
} camera_ortho_state;

typedef struct {
    f32 fov, aspect, near, far;
} camera_persp_state;

typedef struct {
    mat4 projection;
    mat4 view;
    vec2 resolution;
    transform tran;
    union {
        camera_ortho_state ortho;
        camera_persp_state persp;
    };
} camera;

void init_camera(camera* cam, vec2 resolution);
void set_camera_ortho_mat4(camera* cam);
void set_camera_persp_mat4(camera* cam);
void translate_camera(camera* cam, vec3 translation);
void set_camera_position(camera* cam, vec3 position);

#endif
