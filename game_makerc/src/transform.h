#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_
#include <cglm/cglm.h>

typedef struct transform transform;
struct transform {
    transform* parent;
    vec3 position, euler_angle, scale;
    vec3 local_position, local_euler_angle;
    vec3 right, up, forward;
};

void init_transform(transform* tran);
void tran_translate(transform* tran, vec3 translation);
void tran_copy(transform* src, transform* dest);
void tran_rotate(transform* tran, vec3 euler_angle);

#endif
