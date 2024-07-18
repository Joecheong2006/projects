#ifndef _TRANSFORM_
#define _TRANSFORM_
#include <cglm/cglm.h>

typedef struct transform transform;
struct transform {
    transform* parent;
    vec3 position, local_position;
    vec3 scale;
};

void init_transform(transform* tran);
void tran_translate(transform* tran, vec3 translation);
void tran_copy(transform* src, transform* dest);

#endif
