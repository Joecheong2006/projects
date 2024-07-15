#include "camera2d.h"

void init_camera2d(camera2d* cam, vec2 canvas) {
    glm_mat4_identity(cam->ortho);
    glm_mat4_identity(cam->view);
    glm_vec2_copy(canvas, cam->canvas);
    set_camera_ortho_mat4(cam->ortho, canvas);
}

void set_camera_ortho_mat4(mat4 ortho, vec2 canvas) {
    glm_ortho(-canvas[0], canvas[0], -canvas[1], canvas[1], -1, 1, ortho);
}

void translate_camera2d(camera2d* cam, vec2 translation) {
    glm_translate(cam->view, (vec3){-translation[0], -translation[1], 0});
}
