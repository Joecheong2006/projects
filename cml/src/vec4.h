#ifndef VEC4_H
#define VEC4_H

#define DEFINE_VEC4(T, suffix)\
    typedef T vec4##suffix[4];\

#define SET_DEFAULT_VEC4(suffix) typedef vec4##suffix vec4

#define DEFINE_BASIC_VEC4_BEHAVIOUR(T, suffix)\
    CML_INLINE\
    void vec4##suffix##_copy(vec4##suffix dest, vec4##suffix src) {\
        memcpy(dest, src, sizeof(T) * 4);\
    }\
    CML_INLINE\
    T vec4##suffix##_dot(vec4##suffix v1, vec4##suffix v2) {\
        return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2] + v1[3] * v2[3];\
    }\
    CML_INLINE\
    void vec4##suffix##_mul(vec4##suffix v, f32 value) {\
        v[0] *= value;\
        v[1] *= value;\
        v[2] *= value;\
        v[3] *= value;\
    }\
    CML_INLINE\
    f32 vec4##suffix##_length(vec4##suffix v) {\
        return sqrtf(vec4##suffix##_dot(v, v));\
    }\
    CML_INLINE\
    void vec4##suffix##_normalize(vec4##suffix v) {\
        f32 il = 1.0 / vec4##suffix##_length(v);\
        v[0] *= il;\
        v[1] *= il;\
        v[2] *= il;\
        v[3] *= il;\
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
    CML_INLINE\
    void vec4_mul(vec4 v, f32 value) {\
        v[0] *= value;\
        v[1] *= value;\
        v[2] *= value;\
        v[3] *= value;\
    }\
    CML_INLINE\
    f32 vec4_length(vec4 v) {\
        return sqrtf(vec4_dot(v, v));\
    }\
    CML_INLINE\
    void vec4_normalize(vec4 v) {\
        f32 il = 1.0 / vec4_length(v);\
        v[0] *= il;\
        v[1] *= il;\
        v[2] *= il;\
        v[3] *= il;\
    }\

#define SET_VEC4(T, suffix)\
    DEFINE_VEC4(T, suffix)\
    DEFINE_BASIC_VEC4_BEHAVIOUR(T, suffix)

#endif
