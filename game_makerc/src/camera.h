#ifndef _CAMERA2D_
#define _CAMERA2D_
#include <cglm/cglm.h>

typedef struct {
    mat4 projection;
    mat4 view;
    vec2 canvas;
    vec3 position;
} camera;

void init_camera(camera* cam, vec2 canvas);
void set_camera_ortho_mat4(mat4 ortho, vec2 canvas);
void translate_camera(camera* cam, vec3 translation);
void set_camera_position(camera* cam, vec3 position);

#endif
