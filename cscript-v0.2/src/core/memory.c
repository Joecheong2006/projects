#include "memory.h"
#include <stdlib.h>

static int allocation_count = 0;

void* new_mem(size_t _Size) {
#if defined(DEBUG) || defined(_DEBUG)
    ++allocation_count;
#endif
    return malloc(_Size);
}

void* cnew_mem(size_t _NumOfElements, size_t _SizeOfElements) {
#if defined(DEBUG) || defined(_DEBUG)
    ++allocation_count;
#endif
    return calloc(_NumOfElements, _SizeOfElements);
}

void* renew_mem(void *_Memory,size_t _NewSize) {
    return realloc(_Memory, _NewSize);
}

void free_mem(void *_Memory) {
#if defined(DEBUG) || defined(_DEBUG)
    --allocation_count;
#endif
    free(_Memory);
}

int get_allocation_count(void) {
    return allocation_count;
}
