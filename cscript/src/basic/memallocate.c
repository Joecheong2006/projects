#include "memallocate.h"

__attribute__((unused))
static int allocation_count = 0;

void* memallocate(size_t _Size) { ++allocation_count; return malloc(_Size); }
void* cmemallocate(size_t _NumOfElements, size_t _SizeOfElements) { ++allocation_count; return calloc(_NumOfElements, _SizeOfElements); }
void freemem(void *_Memory) { --allocation_count; free(_Memory); }

int check_memory_leak(void) { return allocation_count; }
