#ifndef CML_VEC_H
#define CML_VEC_H

#define DEFINE_VEC(T, suffix)\
    SET_VEC3(T, suffix)\
    SET_VEC4(T, suffix)

#define DEFINE_DEFAULT_VEC(T, suffix)\
    SET_DEFAULT_VEC3(suffix);\
    SET_DEFAULT_VEC4(suffix);\
    DEFINE_DEFAULT_BASIC_VEC3_BEHAVIOUR(T)\
    DEFINE_DEFAULT_BASIC_VEC4_BEHAVIOUR(T)

#endif
