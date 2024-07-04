#ifndef _ERROR_H
#define _ERROR_H

typedef enum {
	ErrorNone,
	ErrorInvalidStringSize,
	ErrorInvalidParam,
	ErrorMissingParam,
	ErrorMallocFaild,
	ErrorOpenFile,
	ErrorInvalidType,
} error_type;

#endif
