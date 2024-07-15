#ifndef _CAMERA2D_
#define _CAMERA2D_
#include <cglm/cglm.h>

typedef struct {
    mat4 ortho;
    mat4 view;
    vec2 canvas;
} camera2d;

void init_camera2d(camera2d* cam, vec2 canvas);
void set_camera_ortho_mat4(mat4 ortho, vec2 canvas);
void translate_camera2d(camera2d* cam, vec2 translation);

#endif
