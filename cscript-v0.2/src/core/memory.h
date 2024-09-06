#ifndef _CORE_MEMORY_H_
#define _CORE_MEMORY_H_
#include "core/defines.h"

void* new_mem(size_t _Size);
void* cnew_mem(size_t _NumOfElements, size_t _SizeOfElements);
void* renew_mem(void *_Memory,size_t _NewSize);
void free_mem(void *_Memory);

int get_allocation_count(void);

#endif
