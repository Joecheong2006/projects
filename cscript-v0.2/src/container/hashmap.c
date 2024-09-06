#include "hashmap.h"
#include "core/assert.h"

hashmap make_hashmap(u32 size, u32 (*hash_function)(void* data, u32 size)) {
    hashmap result;
    result.data = make_vector(void*);
    result.size = size;
    result.hash = hash_function;
    for (u32 i = 0; i < result.size; ++i) {
        vector(void*) v = make_vector(void*);
        vector_push(result.data, v);
    }
    return result;
}

void hashmap_add(hashmap* map, void* data) {
    u32 key = map->hash(data, map->size);
    ASSERT(key < map->size);
    vector_push(map->data[key], data);
}

vector(void*) hashmap_access_vector(hashmap* map, void* data) {
    u32 index = map->hash(data, map->size);
    ASSERT(index < map->size);
    return map->data[index];
}

void hashmap_free_items(hashmap map, void(free_item)(void* data)) {
    for (u32 i = 0; i < map.size; ++i) {
        for (u32 j = 0; j < vector_size(map.data[i]); ++j) {
            free_item(map.data[i][j]);
        }
    }
}

void free_hashmap(hashmap* map) {
    ASSERT(map != NULL);
    for (u32 i = 0; i < map->size; ++i) {
        free_vector(map->data[i]);
    }
    free_vector(map->data);
    map->data = NULL;
}

u32 djb2(const char* str) {
    u32 result = 5381;
    for (i32 i = 0; str[i] != 0; ++i) {
        result = ((result << 5) + result) + str[i];
    }
    return result;
}

u32 sdbm(const char* str) {
    u32 result = 0;
    for (i32 i = 0; str[i] != 0; ++i) {
        result = (result << 6) + (result << 16) + str[i] - result;
    }
    return result;
}

