#ifndef ERROR_H
#define ERROR_H

typedef enum {
	ErrorNone,
	ErrorInvalidStringSize,
	ErrorInvalidParam,
	ErrorMallocFaild,
	ErrorOpenFile,
} error_type;

void log_error(error_type et, char* msg);

#endif
