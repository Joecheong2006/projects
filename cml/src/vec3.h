#ifndef VEC3_H
#define VEC3_H

#define DEFINE_VEC3(T, suffix)\
    typedef T vec3##suffix[3];\

#define SET_DEFAULT_VEC3(suffix) typedef vec3##suffix vec3

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
    }\
    CML_INLINE\
    void vec3##suffix##_mul(vec3##suffix v, T value) {\
        v[0] *= value;\
        v[1] *= value;\
        v[2] *= value;\
    }\
    CML_INLINE\
    f32 vec3##suffix##_length(vec3##suffix v) {\
        return sqrtf(vec3##suffix##_dot(v, v));\
    }\
    CML_INLINE\
    void vec3##suffix##_normalize(vec3##suffix v) {\
        f32 il = 1.0 / vec3##suffix##_length(v);\
        v[0] *= il;\
        v[1] *= il;\
        v[2] *= il;\
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
    CML_INLINE\
    void vec3_mul(vec3 v, T value) {\
        v[0] *= value;\
        v[1] *= value;\
        v[2] *= value;\
    }\
    CML_INLINE\
    f32 vec3_length(vec3 v) {\
        return sqrtf(vec3_dot(v, v));\
    }\
    CML_INLINE\
    void vec3_normalize(vec3 v) {\
        f32 il = 1.0 / vec3_length(v);\
        v[0] *= il;\
        v[1] *= il;\
        v[2] *= il;\
    }\

#define SET_VEC3(T, suffix)\
    DEFINE_VEC3(T, suffix)\
    DEFINE_BASIC_VEC3_BEHAVIOUR(T, suffix)

#endif
