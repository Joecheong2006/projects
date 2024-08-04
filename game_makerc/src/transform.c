#include "transform.h"
#include "core/defines.h"
#include <string.h>

void init_transform(transform* tran) {
	ASSERT(tran != NULL);
    memset(tran, 0, sizeof(transform));
    glm_vec3_copy((vec3){1, 1, 1}, tran->scale);
    tran->forward[2] = -1;
    tran->up[1] = 1;
    tran->right[0] = 1;
}

void tran_translate(transform* tran, vec3 translation) {
	ASSERT(tran != NULL);
	glm_vec3_add(tran->position, translation, tran->position);
	glm_vec3_add(tran->local_position, translation, tran->local_position);
}

void tran_copy(transform* src, transform* dest) {
	ASSERT(src != NULL);
	dest->parent = src->parent;
	glm_vec3_copy(src->local_position, dest->local_position);
	glm_vec3_copy(src->position, dest->position);
	glm_vec3_copy(src->scale, dest->scale);
}

void tran_rotate(transform* tran, vec3 euler_angle) {
	ASSERT(tran != NULL);
	glm_vec3_add(tran->euler_angle, euler_angle, tran->euler_angle);
	mat4 m;
	mat3 m3;
	glm_euler(euler_angle, m);
	glm_mat4_pick3(m, m3);
	glm_mat3_mulv(m3, tran->forward, tran->forward);
	glm_mat3_mulv(m3, tran->up, tran->up);
	glm_mat3_mulv(m3, tran->right, tran->right);
}

