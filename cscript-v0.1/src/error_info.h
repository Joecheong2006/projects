#ifndef _ERROR_INFO_H_
#define _ERROR_INFO_H_
#include "core/defines.h"

struct error_info {
    i32 line, position;
    const char* msg;
};

typedef struct error_info error_info;

#endif
