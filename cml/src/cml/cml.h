#ifndef CML_H
#define CML_H

#include "common.h"
#include "util.h"

#include "vec3.h"
#include "vec4.h"
#include "vec.h"
#include "mat3.h"
#include "mat4.h"
#include "mat.h"


DEFINE_VEC(u8, b)
DEFINE_VEC(f64, l)
DEFINE_VEC(f32, f)
DEFINE_MAT(u8, b)
DEFINE_MAT(f64, l)
DEFINE_MAT(f32, f)

DEFINE_DEFAULT_VEC(f32, f)
DEFINE_DEFAULT_MAT(f32, f)

CML_INLINE
f32 radians(f32 degrees) { return PI180 * degrees; }

//    void vec3_copy(vec3* dest, vec3* src);
//    void vec3_cross(vec3* out, vec3* v1, vec3* v2);
//    T vec3_dot(vec3* v1, vec3* v2);
//    void vec4_copy(vec4* dest, vec4* src);
//    T vec4_dot(vec4* v1, vec4* v2);
//    void mat3_zero(mat3* m);
//    void mat3_copy(mat3* dest, mat3* src);
//    void mat3_mul_vec3(vec3* out, mat3* m, vec3* v);
//    void mat3_mul_mat3(mat3* out, mat3* m1, mat3* m2);
//    void mat4_zero(mat4* m);
//    void mat4_copy(mat4* dest, mat4* src);
//    void mat4_mul_vec4(vec4* out, mat4* m, vec4* v);
//    void mat4_mul_mat4(mat4* out, mat4* m1, mat4* m2);

#endif
