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
    typedef union vec3##suffix{\
        struct {\
            T x, y, z;\
        };\
        struct {\
            T r, g, b;\
        };\
    } vec3##suffix;

#define DEFINE_VEC4(T, suffix)\
    typedef union {\
        struct {\
            T x, y, z, w;\
        };\
        struct {\
            T r, g, b, a;\
        };\
    } vec4##suffix;

#define DEFINE_MAT3(suffix)\
    typedef struct {\
        vec3##suffix col[3];\
    } mat3##suffix;

#define DEFINE_MAT4(suffix)\
    typedef struct {\
        vec4##suffix col[4];\
    } mat4##suffix;

#define SET_DEFAULT_VEC3(suffix) typedef vec3##suffix vec3
#define SET_DEFAULT_VEC4(suffix) typedef vec4##suffix vec4

#define SET_DEFAULT_MAT3(suffix) typedef mat3##suffix mat3
#define SET_DEFAULT_MAT4(suffix) typedef mat4##suffix mat4

#define DEFINE_BASIC_VEC3_BEHAVIOUR(T, suffix)\
    CML_INLINE\
    void vec3##suffix##_copy(vec3##suffix* dest, vec3##suffix* src) {\
        memcpy(dest, src, sizeof(T) * 3);\
    }\
    CML_INLINE\
    void vec3##suffix##_cross(vec3##suffix* out, vec3##suffix* v1, vec3##suffix* v2) {\
        out->x = v1->y * v2->z - v1->z * v2->y;\
        out->y = v1->z * v2->x - v1->x * v2->z;\
        out->z = v1->x * v2->y - v1->y * v2->x;\
    }\
    CML_INLINE\
    T vec3##suffix##_dot(vec3##suffix* v1, vec3##suffix* v2) {\
        return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;\
    }

#define DEFINE_BASIC_VEC4_BEHAVIOUR(T, suffix)\
    CML_INLINE\
    void vec4##suffix##_copy(vec4##suffix* dest, vec4##suffix* src) {\
        memcpy(dest, src, sizeof(T) * 4);\
    }\
    CML_INLINE\
    T vec4##suffix##_dot(vec4##suffix* v1, vec4##suffix* v2) {\
        return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z + v1->w * v2->w;\
    }\

#define DEFINE_BASIC_MAT3_BEHAVIOUR(T, suffix)\
    CML_INLINE\
    void mat3##suffix##_zero(mat3##suffix* m) {\
        memset(&m->col[0], 0, sizeof(T) * 3);\
        memset(&m->col[1], 0, sizeof(T) * 3);\
        memset(&m->col[2], 0, sizeof(T) * 3);\
    }\
    CML_INLINE\
    void mat3##suffix##_copy(mat3##suffix* dest, mat3##suffix* src) {\
        vec3##suffix##_copy(&dest->col[0], &src->col[0]);\
        vec3##suffix##_copy(&dest->col[1], &src->col[1]);\
        vec3##suffix##_copy(&dest->col[2], &src->col[2]);\
    }\
    CML_INLINE\
    void mat3##suffix##_mul_vec3##suffix(vec3##suffix* out, mat3##suffix* m, vec3##suffix* v) {\
        out->x = vec3##suffix##_dot(&m->col[0], v);\
        out->y = vec3##suffix##_dot(&m->col[1], v);\
        out->z = vec3##suffix##_dot(&m->col[2], v);\
    }\
    CML_INLINE\
    void mat3##suffix##_mul_mat3##suffix(mat3##suffix* out, mat3##suffix* m1, mat3##suffix* m2) {\
        vec3##suffix temp = { { m2->col[0].x,  m2->col[1].x,  m2->col[2].x } };\
        out->col[0].x = vec3##suffix##_dot(&m1->col[0], &temp);\
        out->col[1].x = vec3##suffix##_dot(&m1->col[1], &temp);\
        out->col[2].x = vec3##suffix##_dot(&m1->col[2], &temp);\
        temp = (vec3##suffix){ { m2->col[0].y,  m2->col[1].y,  m2->col[2].y } };\
        out->col[0].y = vec3##suffix##_dot(&m1->col[0], &temp);\
        out->col[1].y = vec3##suffix##_dot(&m1->col[1], &temp);\
        out->col[2].y = vec3##suffix##_dot(&m1->col[2], &temp);\
        temp = (vec3##suffix){ { m2->col[0].z,  m2->col[1].z,  m2->col[2].z } };\
        out->col[0].z = vec3##suffix##_dot(&m1->col[0], &temp);\
        out->col[1].z = vec3##suffix##_dot(&m1->col[1], &temp);\
        out->col[2].z = vec3##suffix##_dot(&m1->col[2], &temp);\
    }\

#define DEFINE_BASIC_MAT4_BEHAVIOUR(T, suffix)\
    CML_INLINE\
    void mat4##suffix##_zero(mat4##suffix* m) {\
        memset(&m->col[0], 0, sizeof(T) * 4);\
        memset(&m->col[1], 0, sizeof(T) * 4);\
        memset(&m->col[2], 0, sizeof(T) * 4);\
        memset(&m->col[3], 0, sizeof(T) * 4);\
    }\
    CML_INLINE\
    void mat4##suffix##_copy(mat4##suffix* dest, mat4##suffix* src) {\
        vec4##suffix##_copy(&dest->col[0], &src->col[0]);\
        vec4##suffix##_copy(&dest->col[1], &src->col[1]);\
        vec4##suffix##_copy(&dest->col[2], &src->col[2]);\
        vec4##suffix##_copy(&dest->col[3], &src->col[3]);\
    }\
    CML_INLINE\
    void mat4##suffix##_mul_vec4##suffix(vec4##suffix* out, mat4##suffix* m, vec4##suffix* v) {\
        out->x = vec4##suffix##_dot(&m->col[0], v);\
        out->y = vec4##suffix##_dot(&m->col[1], v);\
        out->z = vec4##suffix##_dot(&m->col[2], v);\
        out->w = vec4##suffix##_dot(&m->col[3], v);\
    }\
    CML_INLINE\
    void mat4##suffix##_mul_mat4##suffix(mat4##suffix* out, mat4##suffix* m1, mat4##suffix* m2) {\
        vec4##suffix temp = { { m2->col[0].x,  m2->col[1].x,  m2->col[2].x, m2->col[3].x } };\
        out->col[0].x = vec4##suffix##_dot(&m1->col[0], &temp);\
        out->col[1].x = vec4##suffix##_dot(&m1->col[1], &temp);\
        out->col[2].x = vec4##suffix##_dot(&m1->col[2], &temp);\
        out->col[3].x = vec4##suffix##_dot(&m1->col[3], &temp);\
        temp = (vec4##suffix){ { m2->col[0].y,  m2->col[1].y,  m2->col[2].y, m2->col[3].y } };\
        out->col[0].y = vec4##suffix##_dot(&m1->col[0], &temp);\
        out->col[1].y = vec4##suffix##_dot(&m1->col[1], &temp);\
        out->col[2].y = vec4##suffix##_dot(&m1->col[2], &temp);\
        out->col[3].y = vec4##suffix##_dot(&m1->col[3], &temp);\
        temp = (vec4##suffix){ { m2->col[0].z,  m2->col[1].z,  m2->col[2].z, m2->col[3].z } };\
        out->col[0].z = vec4##suffix##_dot(&m1->col[0], &temp);\
        out->col[1].z = vec4##suffix##_dot(&m1->col[1], &temp);\
        out->col[2].z = vec4##suffix##_dot(&m1->col[2], &temp);\
        out->col[3].z = vec4##suffix##_dot(&m1->col[3], &temp);\
        temp = (vec4##suffix){ { m1->col[0].w,  m2->col[1].w,  m2->col[2].w, m2->col[3].w } };\
        out->col[0].w = vec4##suffix##_dot(&m1->col[0], &temp);\
        out->col[1].w = vec4##suffix##_dot(&m1->col[1], &temp);\
        out->col[2].w = vec4##suffix##_dot(&m1->col[2], &temp);\
        out->col[3].w = vec4##suffix##_dot(&m1->col[3], &temp);\
    }\

#define DEFINE_DEFAULT_BASIC_VEC3_BEHAVIOUR(T)\
    CML_INLINE\
    void vec3_copy(vec3* dest, vec3* src) {\
        memcpy(dest, src, sizeof(T) * 3);\
    }\
    CML_INLINE\
    void vec3_cross(vec3* out, vec3* v1, vec3* v2) {\
        out->x = v1->y * v2->z - v1->z * v2->y;\
        out->y = v1->z * v2->x - v1->x * v2->z;\
        out->z = v1->x * v2->y - v1->y * v2->x;\
    }\
    CML_INLINE\
    T vec3_dot(vec3* v1, vec3* v2) {\
        return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;\
    }\

#define DEFINE_DEFAULT_BASIC_VEC4_BEHAVIOUR(T)\
    CML_INLINE\
    void vec4_copy(vec4* dest, vec4* src) {\
        memcpy(dest, src, sizeof(T) * 4);\
    }\
    CML_INLINE\
    T vec4_dot(vec4* v1, vec4* v2) {\
        return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z + v1->w * v2->w;\
    }\

#define DEFINE_DEFAULT_BASIC_MAT3_BEHAVIOUR(T)\
    CML_INLINE\
    void mat3_zero(mat3* m) {\
        memset(&m->col[0], 0, sizeof(T) * 3);\
        memset(&m->col[1], 0, sizeof(T) * 3);\
        memset(&m->col[2], 0, sizeof(T) * 3);\
    }\
    CML_INLINE\
    void mat3_copy(mat3* dest, mat3* src) {\
        vec3_copy(&dest->col[0], &src->col[0]);\
        vec3_copy(&dest->col[1], &src->col[1]);\
        vec3_copy(&dest->col[2], &src->col[2]);\
    }\
    CML_INLINE\
    void mat3_mul_vec3(vec3* out, mat3* m, vec3* v) {\
        out->x = vec3_dot(&m->col[0], v);\
        out->y = vec3_dot(&m->col[1], v);\
        out->z = vec3_dot(&m->col[2], v);\
    }\
    CML_INLINE\
    void mat3_mul_mat3(mat3* out, mat3* m1, mat3* m2) {\
        vec3 temp = INIT_VEC3(m2->col[0].x, m2->col[1].x, m2->col[2].x);\
        out->col[0].x = vec3_dot(&m1->col[0], &temp);\
        out->col[1].x = vec3_dot(&m1->col[1], &temp);\
        out->col[2].x = vec3_dot(&m1->col[2], &temp);\
        temp = INIT_VEC3(m2->col[0].y, m2->col[1].y, m2->col[2].y);\
        out->col[0].y = vec3_dot(&m1->col[0], &temp);\
        out->col[1].y = vec3_dot(&m1->col[1], &temp);\
        out->col[2].y = vec3_dot(&m1->col[2], &temp);\
        temp = INIT_VEC3(m2->col[0].z, m2->col[1].z, m2->col[2].z);\
        out->col[0].z = vec3_dot(&m1->col[0], &temp);\
        out->col[1].z = vec3_dot(&m1->col[1], &temp);\
        out->col[2].z = vec3_dot(&m1->col[2], &temp);\
    }\


#define DEFINE_DEFAULT_BASIC_MAT4_BEHAVIOUR(T)\
    CML_INLINE\
    void mat4_zero(mat4* m) {\
        memset(&m->col[0], 0, sizeof(T) * 4);\
        memset(&m->col[1], 0, sizeof(T) * 4);\
        memset(&m->col[2], 0, sizeof(T) * 4);\
        memset(&m->col[3], 0, sizeof(T) * 4);\
    }\
    CML_INLINE\
    void mat4_copy(mat4* dest, mat4* src) {\
        vec4_copy(&dest->col[0], &src->col[0]);\
        vec4_copy(&dest->col[1], &src->col[1]);\
        vec4_copy(&dest->col[2], &src->col[2]);\
        vec4_copy(&dest->col[3], &src->col[3]);\
    }\
    CML_INLINE\
    void mat4_mul_vec4(vec4* out, mat4* m, vec4* v) {\
        out->x = vec4_dot(&m->col[0], v);\
        out->y = vec4_dot(&m->col[1], v);\
        out->z = vec4_dot(&m->col[2], v);\
        out->w = vec4_dot(&m->col[3], v);\
    }\
    CML_INLINE\
    void mat4_mul_mat4(mat4* out, mat4* m1, mat4* m2) {\
        vec4 temp = INIT_VEC4(m2->col[0].x, m2->col[1].x, m2->col[2].x, m2->col[3].x);\
        out->col[0].x = vec4_dot(&m1->col[0], &temp);\
        out->col[1].x = vec4_dot(&m1->col[1], &temp);\
        out->col[2].x = vec4_dot(&m1->col[2], &temp);\
        out->col[3].x = vec4_dot(&m1->col[3], &temp);\
        temp = INIT_VEC4(m2->col[0].y, m2->col[1].y, m2->col[2].y, m2->col[3].y);\
        out->col[0].y = vec4_dot(&m1->col[0], &temp);\
        out->col[1].y = vec4_dot(&m1->col[1], &temp);\
        out->col[2].y = vec4_dot(&m1->col[2], &temp);\
        out->col[3].y = vec4_dot(&m1->col[3], &temp);\
        temp = INIT_VEC4(m2->col[0].z, m2->col[1].z, m2->col[2].z, m2->col[3].z);\
        out->col[0].z = vec4_dot(&m1->col[0], &temp);\
        out->col[1].z = vec4_dot(&m1->col[1], &temp);\
        out->col[2].z = vec4_dot(&m1->col[2], &temp);\
        out->col[3].z = vec4_dot(&m1->col[3], &temp);\
        temp = INIT_VEC4(m2->col[0].w, m2->col[1].w, m2->col[2].w, m2->col[3].w);\
        out->col[0].w = vec4_dot(&m1->col[0], &temp);\
        out->col[1].w = vec4_dot(&m1->col[1], &temp);\
        out->col[2].w = vec4_dot(&m1->col[2], &temp);\
        out->col[3].w = vec4_dot(&m1->col[3], &temp);\
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

#define INIT_VEC3i(x, y, z) (vec3i){ { x, y, z } }
#define INIT_VEC4i(x, y, z, w) (vec4i){ { x, y, z, w } }
DEFINE_VEC(i32, i)
DEFINE_MAT(i32, i)

#define INIT_VEC3b(x, y, z) (vec3b){ { x, y, z } }
#define INIT_VEC4b(x, y, z, w) (vec4b){ { x, y, z, w } }
DEFINE_VEC(u8, b)
DEFINE_MAT(u8, b)

#define INIT_VEC3(x, y, z) (vec3){ { x, y, z } }
#define INIT_VEC4(x, y, z, w) (vec4){ { x, y, z, w } }
DEFINE_DEFAULT_VEC(f32, f)
DEFINE_DEFAULT_MAT(f32, f)

#endif
