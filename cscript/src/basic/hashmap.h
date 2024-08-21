#ifndef HASHMAP_H
#define HASHMAP_H
#include "vector.h"
#include <stddef.h>

typedef struct {
    size_t size;
    vector(vector(void*)) data;
    u64(*hash)(void* data, u64 size);
} hashmap;

hashmap make_hashmap(size_t size, u64(*hash_function)(void* data, u64 size));
void hashmap_add(hashmap map, void* data);
vector(void*) hashmap_access_vector(hashmap map, void* data);
void hashmap_free_items(hashmap map, void(free_item)(void* data));
void free_hashmap(hashmap* map);

size_t djb2(const char* str);
size_t sdbm(const char* str);

#endif
