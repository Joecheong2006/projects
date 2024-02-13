#ifndef CML_H
#define CML_H

#include "util.h"
#include <string.h>

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

#define CML_INLINE static inline __attribute((always_inline))

#define DEFINE_VEC3(T, suffix)\
    typedef T vec3##suffix[3];\

#define DEFINE_VEC4(T, suffix)\
    typedef T vec4##suffix[4];\

#define DEFINE_MAT3(suffix)\
    typedef vec3##suffix mat3##suffix[3];

#define DEFINE_MAT4(suffix)\
    typedef vec4##suffix mat4##suffix[4];

#define SET_DEFAULT_VEC3(suffix) typedef vec3##suffix vec3
#define SET_DEFAULT_VEC4(suffix) typedef vec4##suffix vec4

#define SET_DEFAULT_MAT3(suffix) typedef mat3##suffix mat3
#define SET_DEFAULT_MAT4(suffix) typedef mat4##suffix mat4

#define DEFINE_BASIC_VEC3_BEHAVIOUR(T, suffix)\
    CML_INLINE\
    void vec3##suffix##_copy(vec3##suffix dest, vec3##suffix src) {\
        memcpy(dest, src, sizeof(T) * 3);\
    }\
    CML_INLINE\
    void vec3##suffix##_cross(vec3##suffix out, vec3##suffix v1, vec3##suffix v2) {\
        out[0] = v1[1] * v2[2] - v1[2] * v2[1];\
        out[1] = v1[2] * v2[0] - v1[0] * v2[2];\
        out[2] = v1[0] * v2[1] - v1[1] * v2[0];\
    }\
    CML_INLINE\
    T vec3##suffix##_dot(vec3##suffix v1, vec3##suffix v2) {\
        return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];\
    }

#define DEFINE_BASIC_VEC4_BEHAVIOUR(T, suffix)\
    CML_INLINE\
    void vec4##suffix##_copy(vec4##suffix dest, vec4##suffix src) {\
        memcpy(dest, src, sizeof(T) * 4);\
    }\
    CML_INLINE\
    T vec4##suffix##_dot(vec4##suffix v1, vec4##suffix v2) {\
        return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2] + v1[3] * v2[3];\
    }\

#define DEFINE_BASIC_MAT3_BEHAVIOUR(T, suffix)\
    CML_INLINE\
    void mat3##suffix##_zero(mat3##suffix m) {\
        memset(m[0], 0, sizeof(T) * 3);\
        memset(m[1], 0, sizeof(T) * 3);\
        memset(m[2], 0, sizeof(T) * 3);\
    }\
    CML_INLINE\
    void mat3##suffix##_copy(mat3##suffix dest, mat3##suffix src) {\
        vec3##suffix##_copy(dest[0], src[0]);\
        vec3##suffix##_copy(dest[1], src[1]);\
        vec3##suffix##_copy(dest[2], src[2]);\
    }\
    CML_INLINE\
    void mat3##suffix##_mul_vec3##suffix(vec3##suffix out, mat3##suffix m, vec3##suffix v) {\
        out[0] = vec3##suffix##_dot(m[0], v);\
        out[1] = vec3##suffix##_dot(m[1], v);\
        out[2] = vec3##suffix##_dot(m[2], v);\
    }\
    CML_INLINE\
    void mat3##suffix##_mul_mat3##suffix(mat3##suffix out, mat3##suffix m1, mat3##suffix m2) {\
        {\
        vec3##suffix temp = { m2[0][0],  m2[1][0],  m2[2][0] };\
        out[0][0] = vec3##suffix##_dot(m1[0], temp);\
        out[1][0] = vec3##suffix##_dot(m1[1], temp);\
        out[2][0] = vec3##suffix##_dot(m1[2], temp);\
        }\
        {\
        vec3##suffix temp = { m2[0][1],  m2[1][1],  m2[2][1] };\
        out[0][1] = vec3##suffix##_dot(m1[0], temp);\
        out[1][1] = vec3##suffix##_dot(m1[1], temp);\
        out[2][1] = vec3##suffix##_dot(m1[2], temp);\
        }\
        {\
        vec3##suffix temp = { m2[0][2],  m2[1][2],  m2[2][2] };\
        out[0][2] = vec3##suffix##_dot(m1[0], temp);\
        out[1][2] = vec3##suffix##_dot(m1[1], temp);\
        out[2][2] = vec3##suffix##_dot(m1[2], temp);\
        }\
    }\

#define DEFINE_BASIC_MAT4_BEHAVIOUR(T, suffix)\
    CML_INLINE\
    void mat4##suffix##_zero(mat4##suffix m) {\
        memset(m[0], 0, sizeof(T) * 4);\
        memset(m[1], 0, sizeof(T) * 4);\
        memset(m[2], 0, sizeof(T) * 4);\
        memset(m[3], 0, sizeof(T) * 4);\
    }\
    CML_INLINE\
    void mat4##suffix##_copy(mat4##suffix dest, mat4##suffix src) {\
        vec4##suffix##_copy(dest[0], src[0]);\
        vec4##suffix##_copy(dest[1], src[1]);\
        vec4##suffix##_copy(dest[2], src[2]);\
        vec4##suffix##_copy(dest[3], src[3]);\
    }\
    CML_INLINE\
    void mat4##suffix##_mul_vec4##suffix(vec4##suffix out, mat4##suffix m, vec4##suffix v) {\
        out[0] = vec4##suffix##_dot(m[0], v);\
        out[1] = vec4##suffix##_dot(m[1], v);\
        out[2] = vec4##suffix##_dot(m[2], v);\
        out[3] = vec4##suffix##_dot(m[3], v);\
    }\
    CML_INLINE\
    void mat4##suffix##_mul_mat4##suffix(mat4##suffix out, mat4##suffix m1, mat4##suffix m2) {\
        {\
        vec4##suffix temp = { m2[0][0],  m2[1][0],  m2[2][0], m2[3][0] };\
        out[0][0] = vec4##suffix##_dot(m1[0], temp);\
        out[1][0] = vec4##suffix##_dot(m1[1], temp);\
        out[2][0] = vec4##suffix##_dot(m1[2], temp);\
        out[3][0] = vec4##suffix##_dot(m1[3], temp);\
        }\
        {\
        vec4##suffix temp = { m2[0][1],  m2[1][1],  m2[2][1], m2[3][1] };\
        out[0][1] = vec4##suffix##_dot(m1[0], temp);\
        out[1][1] = vec4##suffix##_dot(m1[1], temp);\
        out[2][1] = vec4##suffix##_dot(m1[2], temp);\
        out[3][1] = vec4##suffix##_dot(m1[3], temp);\
        }\
        {\
        vec4##suffix temp = { m2[0][2],  m2[1][2],  m2[2][2], m2[3][2] };\
        out[0][2] = vec4##suffix##_dot(m1[0], temp);\
        out[1][2] = vec4##suffix##_dot(m1[1], temp);\
        out[2][2] = vec4##suffix##_dot(m1[2], temp);\
        out[3][2] = vec4##suffix##_dot(m1[3], temp);\
        }\
        {\
        vec4##suffix temp = { m2[0][3],  m2[1][3],  m2[2][3], m2[3][3] };\
        out[0][3] = vec4##suffix##_dot(m1[0], temp);\
        out[1][3] = vec4##suffix##_dot(m1[1], temp);\
        out[2][3] = vec4##suffix##_dot(m1[2], temp);\
        out[3][3] = vec4##suffix##_dot(m1[3], temp);\
        }\
    }\

#define DEFINE_DEFAULT_BASIC_VEC3_BEHAVIOUR(T)\
    CML_INLINE\
    void vec3_copy(vec3 dest, vec3 src) {\
        memcpy(dest, src, sizeof(T) * 3);\
    }\
    CML_INLINE\
    void vec3_cross(vec3 out, vec3 v1, vec3 v2) {\
        out[0] = v1[1] * v2[2] - v1[2] * v2[1];\
        out[1] = v1[2] * v2[0] - v1[0] * v2[2];\
        out[2] = v1[0] * v2[1] - v1[1] * v2[0];\
    }\
    CML_INLINE\
    T vec3_dot(vec3 v1, vec3 v2) {\
        return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];\
    }\

#define DEFINE_DEFAULT_BASIC_VEC4_BEHAVIOUR(T)\
    CML_INLINE\
    void vec4_copy(vec4 dest, vec4 src) {\
        memcpy(dest, src, sizeof(T) * 4);\
    }\
    CML_INLINE\
    T vec4_dot(vec4 v1, vec4 v2) {\
        return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2] + v1[3] * v2[3];\
    }\

#define DEFINE_DEFAULT_BASIC_MAT3_BEHAVIOUR(T)\
    CML_INLINE\
    void mat3_zero(mat3 m) {\
        memset(m[0], 0, sizeof(T) * 3);\
        memset(m[1], 0, sizeof(T) * 3);\
        memset(m[2], 0, sizeof(T) * 3);\
    }\
    CML_INLINE\
    void mat3_copy(mat3 dest, mat3 src) {\
        vec3_copy(dest[0], src[0]);\
        vec3_copy(dest[1], src[1]);\
        vec3_copy(dest[2], src[2]);\
    }\
    CML_INLINE\
    void mat3_mul_vec3(vec3 out, mat3 m, vec3 v) {\
        out[0] = vec3_dot(m[0], v);\
        out[1] = vec3_dot(m[1], v);\
        out[2] = vec3_dot(m[2], v);\
    }\
    CML_INLINE\
    void mat3_mul_mat3(mat3 out, mat3 m1, mat3 m2) {\
        {\
        vec3 temp = { m2[0][0], m2[1][0], m2[2][0] };\
        out[0][0] = vec3_dot(m1[0], temp);\
        out[1][0] = vec3_dot(m1[1], temp);\
        out[2][0] = vec3_dot(m1[2], temp);\
        }\
        {\
        vec3 temp = { m2[0][1], m2[1][1], m2[2][1] };\
        out[0][1] = vec3_dot(m1[0], temp);\
        out[1][1] = vec3_dot(m1[1], temp);\
        out[2][1] = vec3_dot(m1[2], temp);\
        }\
        {\
        vec3 temp = { m2[0][2], m2[1][2], m2[2][2] };\
        out[0][2] = vec3_dot(m1[0], temp);\
        out[1][2] = vec3_dot(m1[1], temp);\
        out[2][2] = vec3_dot(m1[2], temp);\
        }\
    }\


#define DEFINE_DEFAULT_BASIC_MAT4_BEHAVIOUR(T)\
    CML_INLINE\
    void mat4_zero(mat4 m) {\
        memset(m[0], 0, sizeof(T) * 4);\
        memset(m[1], 0, sizeof(T) * 4);\
        memset(m[2], 0, sizeof(T) * 4);\
        memset(m[3], 0, sizeof(T) * 4);\
    }\
    CML_INLINE\
    void mat4_copy(mat4 dest, mat4 src) {\
        vec4_copy(dest[0], src[0]);\
        vec4_copy(dest[1], src[1]);\
        vec4_copy(dest[2], src[2]);\
        vec4_copy(dest[3], src[3]);\
    }\
    CML_INLINE\
    void mat4_mul_vec4(vec4 out, mat4 m, vec4 v) {\
        out[0] = vec4_dot(m[0], v);\
        out[1] = vec4_dot(m[1], v);\
        out[2] = vec4_dot(m[2], v);\
        out[3] = vec4_dot(m[3], v);\
    }\
    CML_INLINE\
    void mat4_mul_mat4(mat4 out, mat4 m1, mat4 m2) {\
        {\
        vec4 temp = { m2[0][0],  m2[1][0],  m2[2][0], m2[3][0] };\
        out[0][0] = vec4_dot(m1[0], temp);\
        out[1][0] = vec4_dot(m1[1], temp);\
        out[2][0] = vec4_dot(m1[2], temp);\
        out[3][0] = vec4_dot(m1[3], temp);\
        }\
        {\
        vec4 temp = { m2[0][1],  m2[1][1],  m2[2][1], m2[3][1] };\
        out[0][1] = vec4_dot(m1[0], temp);\
        out[1][1] = vec4_dot(m1[1], temp);\
        out[2][1] = vec4_dot(m1[2], temp);\
        out[3][1] = vec4_dot(m1[3], temp);\
        }\
        {\
        vec4 temp = { m2[0][2],  m2[1][2],  m2[2][2], m2[3][2] };\
        out[0][2] = vec4_dot(m1[0], temp);\
        out[1][2] = vec4_dot(m1[1], temp);\
        out[2][2] = vec4_dot(m1[2], temp);\
        out[3][2] = vec4_dot(m1[3], temp);\
        }\
        {\
        vec4 temp = { m2[0][3],  m2[1][3],  m2[2][3], m2[3][3] };\
        out[0][3] = vec4_dot(m1[0], temp);\
        out[1][3] = vec4_dot(m1[1], temp);\
        out[2][3] = vec4_dot(m1[2], temp);\
        out[3][3] = vec4_dot(m1[3], temp);\
        }\
    }\


#define SET_VEC3(T, suffix)\
    DEFINE_VEC3(T, suffix)\
    DEFINE_BASIC_VEC3_BEHAVIOUR(T, suffix)

#define SET_VEC4(T, suffix)\
    DEFINE_VEC4(T, suffix)\
    DEFINE_BASIC_VEC4_BEHAVIOUR(T, suffix)

#define SET_MAT3(T, suffix)\
    DEFINE_MAT3(suffix)\
    DEFINE_BASIC_MAT3_BEHAVIOUR(T, suffix)

#define SET_MAT4(T, suffix)\
    DEFINE_MAT4(suffix)\
    DEFINE_BASIC_MAT4_BEHAVIOUR(T, suffix)

#define DEFINE_VEC(T, suffix)\
    SET_VEC3(T, suffix)\
    SET_VEC4(T, suffix)

#define DEFINE_MAT(T, suffix)\
    SET_MAT3(T, suffix)\
    SET_MAT4(T, suffix)

#define DEFINE_DEFAULT_VEC(T, suffix)\
    DEFINE_VEC(T, suffix)\
    SET_DEFAULT_VEC3(suffix);\
    SET_DEFAULT_VEC4(suffix);\
    DEFINE_DEFAULT_BASIC_VEC3_BEHAVIOUR(T)\
    DEFINE_DEFAULT_BASIC_VEC4_BEHAVIOUR(T)

#define DEFINE_DEFAULT_MAT(T, suffix)\
    DEFINE_MAT(T, suffix)\
    SET_DEFAULT_MAT3(suffix);\
    SET_DEFAULT_MAT4(suffix);\
    DEFINE_DEFAULT_BASIC_MAT3_BEHAVIOUR(T)\
    DEFINE_DEFAULT_BASIC_MAT4_BEHAVIOUR(T)

#define INIT_VEC4i(x, y, z, w) (vec4i){ { x, y, z, w } }
DEFINE_VEC(i32, i)
DEFINE_MAT(i32, i)

#define INIT_VEC4b(x, y, z, w) (vec4b){ { x, y, z, w } }
DEFINE_VEC(u8, b)
DEFINE_MAT(u8, b)

#define INIT_VEC4(x, y, z, w) (vec4){ { x, y, z, w } }
DEFINE_DEFAULT_VEC(f32, f)
DEFINE_DEFAULT_MAT(f32, f)

#endif
