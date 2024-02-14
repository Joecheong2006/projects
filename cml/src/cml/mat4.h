#ifndef MAT4_H
#define MAT4_H

#define DEFINE_MAT4(suffix) typedef vec4##suffix mat4##suffix[4];

#define SET_DEFAULT_MAT4(suffix) typedef mat4##suffix mat4

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
    void mat4##suffix##_mul(mat4##suffix out, mat4##suffix m1, mat4##suffix m2) {\
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
    void mat4##suffix##_translate(mat4##suffix out, mat4##suffix m, vec3##suffix v) {\
        mat4##suffix##_copy(out, m);\
        out[0][3] += v[0];\
        out[1][3] += v[1];\
        out[2][3] += v[2];\
    }\
    void mat4##suffix##_scale(mat4##suffix out, mat4##suffix m, vec3##suffix v) {\
        mat4##suffix##_copy(out, m);\
        out[0][0] *= v[0];\
        out[1][1] *= v[1];\
        out[2][2] *= v[2];\
    }\
    void mat4##suffix##_make_euler_rotate(mat4##suffix out, vec3##suffix angle) {\
        f32 sx = sinf(angle[0]), sy = sinf(angle[1]), sz = sinf(angle[2]);\
        f32 cx = cosf(angle[0]), cy = cosf(angle[1]), cz = cosf(angle[2]);\
        out[0][0] = cy * cz;\
        out[0][1] = cy * sz;\
        out[0][2] = sy;\
        out[1][0] = cx * sz + sx * sy * cz;\
        out[1][1] = cx * cz - sx * sy * sz;\
        out[1][2] = sx * cy;\
        out[2][0] = sx * sz - cx * sy * cz;\
        out[2][1] = sx * cz + cx * sy * sz;\
        out[2][2] = cx * cy;\
    }\
    void mat4##suffix##_rotate(mat4##suffix out, mat4##suffix m, f32 angle, vec3##suffix v) {\
        f32 c = cosf(angle);\
        f32 s = sinf(angle);\
        vec3##suffix##_normalize(v);\
        vec3##suffix temp;\
        vec3##suffix##_copy(temp, v);\
        vec3##suffix##_mul(temp, 1.0 - c);\
        mat4##suffix rotate = MAT4_SET(1);\
        rotate[0][0] = c + temp[0] * v[0];\
        rotate[1][0] = temp[0] * v[1] + s * v[2];\
        rotate[2][0] = temp[0] * v[2] - s * v[1];\
        rotate[0][1] = temp[1] * v[0] - s * v[2];\
        rotate[1][1] = c + temp[1] * v[1];\
        rotate[2][1] = temp[1] * v[2] + s * v[0];\
        rotate[0][2] = temp[2] * v[0] + s * v[1];\
        rotate[1][2] = temp[2] * v[1] - s * v[0];\
        rotate[2][2] = c + temp[2] * v[2];\
        mat4##suffix##_mul(out, rotate, m);\
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
    void mat4_mul(mat4 out, mat4 m1, mat4 m2) {\
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
    void mat4_translate(mat4 out, mat4 m, vec3 v) {\
        mat4_copy(out, m);\
        out[0][3] += v[0];\
        out[1][3] += v[1];\
        out[2][3] += v[2];\
    }\
    void mat4_scale(mat4 out, mat4 m, vec3 v) {\
        mat4_copy(out, m);\
        out[0][0] *= v[0];\
        out[1][1] *= v[1];\
        out[2][2] *= v[2];\
    }\
    void mat4_make_euler_rotate(mat4 dest, vec3 angle) {\
        f32 sx = sinf(angle[0]), sy = sinf(angle[1]), sz = sinf(angle[2]);\
        f32 cx = cosf(angle[0]), cy = cosf(angle[1]), cz = cosf(angle[2]);\
        dest[0][0] = cy * cz;\
        dest[0][1] = cy * sz;\
        dest[0][2] = sy;\
        dest[1][0] = cx * sz + sx * sy * cz;\
        dest[1][1] = cx * cz - sx * sy * sz;\
        dest[1][2] = sx * cy;\
        dest[2][0] = sx * sz - cx * sy * cz;\
        dest[2][1] = sx * cz + cx * sy * sz;\
        dest[2][2] = cx * cy;\
    }\
    void mat4_rotate(mat4 out, mat4 m, f32 angle, vec3 v) {\
        f32 c = cosf(angle);\
        f32 s = sinf(angle);\
        vec3_normalize(v);\
        vec3 temp;\
        vec3_copy(temp, v);\
        vec3_mul(temp, 1.0 - c);\
        mat4 rotate = MAT4_SET(1);\
        rotate[0][0] = c + temp[0] * v[0];\
        rotate[1][0] = temp[0] * v[1] + s * v[2];\
        rotate[2][0] = temp[0] * v[2] - s * v[1];\
        rotate[0][1] = temp[1] * v[0] - s * v[2];\
        rotate[1][1] = c + temp[1] * v[1];\
        rotate[2][1] = temp[1] * v[2] + s * v[0];\
        rotate[0][2] = temp[2] * v[0] + s * v[1];\
        rotate[1][2] = temp[2] * v[1] - s * v[0];\
        rotate[2][2] = c + temp[2] * v[2];\
        mat4_mul(out, rotate, m);\
    }\

#define SET_MAT4(T, suffix)\
    DEFINE_MAT4(suffix)\
    DEFINE_BASIC_MAT4_BEHAVIOUR(T, suffix)

#define MAT4_SET(val) { { val, 0, 0, 0 }, { 0, val, 0, 0 }, { 0, 0, val, 0 }, { 0, 0, 0, val } }

#endif
