#ifndef CML_MAT_H
#define CML_MAT_H

#define DEFINE_MAT(T, suffix)\
    SET_MAT3(T, suffix)\
    SET_MAT4(T, suffix)

#define DEFINE_DEFAULT_MAT(T, suffix)\
    SET_DEFAULT_MAT3(suffix);\
    SET_DEFAULT_MAT4(suffix);\
    DEFINE_DEFAULT_BASIC_MAT3_BEHAVIOUR(T)\
    DEFINE_DEFAULT_BASIC_MAT4_BEHAVIOUR(T)

#endif
