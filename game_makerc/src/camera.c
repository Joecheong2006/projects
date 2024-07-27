#include "camera.h"
#include "util.h"

void init_camera(camera* cam, vec2 canvas) {
    glm_mat4_identity(cam->projection);
    glm_mat4_identity(cam->view);
    glm_vec2_copy(canvas, cam->canvas);
    set_camera_ortho_mat4(cam->projection, canvas);
    glm_vec3_copy((vec3){0, 0, 0}, cam->position);
}

void set_camera_ortho_mat4(mat4 ortho, vec2 canvas) {
    float y = canvas[1] / 5;
    glm_ortho(-canvas[0] / y, canvas[0] / y, -canvas[1] / y, canvas[1] / y, -10, 10, ortho);
}

void translate_camera(camera* cam, vec3 translation) {
    ASSERT(cam != NULL);
    glm_translate(cam->view, (vec3){-translation[0], -translation[1], -translation[2]});
    glm_vec3_add(cam->position, translation, cam->position);
}

void set_camera_position(camera* cam, vec3 position) {
    ASSERT(cam != NULL);
    vec3 offset;
    glm_vec3_sub(position, cam->position, offset);
    glm_translate(cam->view, offset);
    glm_vec3_copy(position, cam->position);
}
