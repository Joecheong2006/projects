#ifndef HASHMAP_H
#define HASHMAP_H
#include "vector.h"

typedef struct {
    u32 size;
    vector(vector(void*)) data;
    u32 (*hash)(void* data, u32 size);
} hashmap;

hashmap make_hashmap(u32 size, u32 (*hash_function)(void* data, u32 size));
void hashmap_add(hashmap* map, void* data);
vector(void*) hashmap_access_vector(hashmap* map, void* data);
void hashmap_free_items(hashmap map, void(free_item)(void* data));
void free_hashmap(hashmap* map);

u32 djb2(const char* str);
u32 sdbm(const char* str);

#endif
