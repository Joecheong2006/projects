#include "camera.h"

void init_camera(camera* cam, vec2 canvas) {
    glm_mat4_identity(cam->projection);
    glm_mat4_identity(cam->view);
    glm_vec2_copy(canvas, cam->canvas);
    set_camera_ortho_mat4(cam->projection, canvas);
}

void set_camera_ortho_mat4(mat4 ortho, vec2 canvas) {
    float y = canvas[1] / 5;
    glm_ortho(-canvas[0] / y, canvas[0] / y, -canvas[1] / y, canvas[1] / y, -1, 1, ortho);
}

void translate_camera(camera* cam, vec3 translation) {
    glm_translate(cam->view, (vec3){-translation[0], -translation[1], -translation[2]});
}
