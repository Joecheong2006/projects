#include "camera.h"

void init_camera(camera* cam, vec2 resolution) {
    glm_mat4_identity(cam->projection);
    glm_mat4_identity(cam->view);
    glm_vec2_copy(resolution, cam->resolution);
    init_transform(&cam->tran);
}

void set_camera_ortho_mat4(camera* cam) {
    f32 aspect = cam->resolution[0] / cam->resolution[1];
    glm_ortho(-cam->ortho.size * aspect, cam->ortho.size * aspect, -cam->ortho.size, cam->ortho.size, cam->ortho.depth[0], cam->ortho.depth[1], cam->projection);
}

void set_camera_persp_mat4(camera* cam) {
    glm_perspective(cam->persp.fov, cam->persp.aspect, cam->persp.near, cam->persp.far, cam->projection);
}

void translate_camera(camera* cam, vec3 translation) {
    ASSERT_MSG(cam != NULL, "invalid camera");
    glm_translate(cam->view, (vec3){-translation[0], -translation[1], -translation[2]});
    glm_vec3_add(cam->tran.position, translation, cam->tran.position);
}

void set_camera_position(camera* cam, vec3 position) {
    ASSERT_MSG(cam != NULL, "invalid camera");
    vec3 offset;
    glm_vec3_sub(position, cam->tran.position, offset);
    glm_translate(cam->view, offset);
    glm_vec3_copy(position, cam->tran.position);
}
