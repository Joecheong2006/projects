#include "hashmap.h"
#include <assert.h>
#include <stdlib.h>

hashmap make_hashmap(size_t size, size_t (*hash_function)(void* data, size_t size)) {
    hashmap result;
    result.data = make_vector();
    result.size = size;
    result.hash = hash_function;
    for (size_t i = 0; i < result.size; ++i) {
        vector_push(result.data, make_vector());
    }
    return result;
}

void hashmap_add(hashmap map, void* data) {
    size_t key = map.hash(data, map.size);
    assert(key < map.size);
    vector_push(map.data[key], data);
}

vector(void*) hashmap_access_vector(hashmap map, void* data) {
    size_t index = map.hash(data, map.size);
    return map.data[index];
}

void hashmap_free_items(hashmap map, void(free_item)(void* data)) {
    for (size_t i = 0; i < map.size; ++i) {
        for (size_t j = 0; j < vector_size(map.data[i]); ++j) {
            free_item(map.data[i][j]);
        }
    }
}

void free_hashmap(hashmap map) {
    for (size_t i = 0; i < map.size; ++i) {
        free_vector(map.data[i]);
    }
    free_vector(map.data);
    map.data = NULL;
}

size_t djb2(const char* str) {
    size_t result = 5381;
    for (i32 i = 0; str[i] != 0; ++i) {
        result = ((result << 5) + result) + str[i];
    }
    return result;
}

size_t sdbm(const char* str) {
    size_t result = 0;
    for (i32 i = 0; str[i] != 0; ++i) {
        result = (result << 6) + (result << 16) + str[i] - result;
    }
    return result;
}

