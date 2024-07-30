#include "camera_shake.h"
#include "camera.h"
#include "game_object_system.h"
#include <GLFW/glfw3.h>
#include <string.h>

static f32 randf(f32 start, f32 end, f32 dr) {
    f32 range = end - start + 1;
    return rand() % (i32)(range / dr) * dr - range * 0.5f;
}

void camera_shake_object_on_start(game_object* obj) {
    camera_shake_object* self = obj->self;
    camera* cam = find_game_object_by_index(0)->self;
    glm_vec3_copy(cam->position, self->origin_pos);
    self->time_start = glfwGetTime();
}

void camera_shake_object_on_update(game_object* obj) {
    static const f32 ds = 0.0001;
    camera_shake_object* self = obj->self;

    camera* cam = find_game_object_by_index(0)->self;

    vec3 t = {randf(-self->strength, self->strength, ds), randf(-self->strength, self->strength, ds), 0};
    glm_vec3_clamp(t, -self->strength, self->strength);
    glm_vec3_add(t, self->origin_pos, t);

    set_camera_position(cam, t);

    if (glfwGetTime() - self->time_start >= self->duration) {
        printf("%g\n", glfwGetTime());
        set_camera_position(cam, self->origin_pos);
        destory_game_object(obj);
    }
}

void create_camera_shake(camera_shake_object* obj) {
    game_object game_obj;
    memset(&game_obj, 0, sizeof(game_object));
    game_obj.self = obj;
    game_obj.on_start = camera_shake_object_on_start;
    game_obj.on_update = camera_shake_object_on_update;
    create_game_object(&game_obj);
}
