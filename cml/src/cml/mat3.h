#ifndef MAT3_H
#define MAT3_H

#define DEFINE_MAT3(suffix) typedef vec3##suffix mat3##suffix[3];

#define SET_DEFAULT_MAT3(suffix) typedef mat3##suffix mat3

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
    void mat3##suffix##_mul(mat3##suffix out, mat3##suffix m1, mat3##suffix m2) {\
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
    void mat3_mul(mat3 out, mat3 m1, mat3 m2) {\
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

#define SET_MAT3(T, suffix)\
    DEFINE_MAT3(suffix)\
    DEFINE_BASIC_MAT3_BEHAVIOUR(T, suffix)

#define MAT3_SET(val) { { val, 0, 0 }, { 0, val, 0 }, { 0, 0, val } }

#endif
