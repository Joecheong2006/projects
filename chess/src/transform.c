#include "transform.h"
#include "util.h"
#include <string.h>

void init_transform(transform* tran) {
	ASSERT(tran != NULL);
    memset(tran, 0, sizeof(transform));
    glm_vec3_copy((vec3){1, 1, 1}, tran->scale);
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
