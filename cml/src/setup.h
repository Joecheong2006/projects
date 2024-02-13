#ifndef CML_SETUP_H
#define CML_SETUP_H


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

#endif
