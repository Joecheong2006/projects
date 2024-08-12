#ifndef MEMALLOCATE_H
#define MEMALLOCATE_H

#include <stdlib.h>

#if defined(DEBUG) || defined(_DEBUG)
#define MALLOC(_Size) memallocate(_Size)
#define CALLOC(_NumOfElements, _SizeOfElements) cmemallocate(_NumOfElements, _SizeOfElements)
#define FREE(_Memory) freemem(_Memory)
void* memallocate(size_t _Size);
void* cmemallocate(size_t _NumOfElements, size_t _SizeOfElements);
void freemem(void *_Memory);
#else
#define MALLOC(_Size) malloc(_Size)
#define CALLOC(_NumOfElements, _SizeOfElements) calloc(_NumOfElements, _SizeOfElements)
#define FREE(_Memory) free(_Memory)
#endif

#include <stdio.h>
int check_memory_leak(void);
#define CHECK_MEMORY_LEAK()\
    int leak_count = check_memory_leak();\
    if (leak_count) {\
        printf("memory leak! count: %d\n", leak_count);\
    }

#endif
