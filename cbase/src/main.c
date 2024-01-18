#include <stdio.h>
#include "vector.h"
#include "util.h"


static void test_vector() {
    vector(int) vec;
    init_vector(&vec);
    vector_reserve(&vec, 2);
    for (u64 i = 0; i < 32; ++i) {
        vector_add(&vec, i);
    }
    for (u64 i = 0; i < vector_status(&vec).size; ++i) {
        printf("%d ", vec[i]);
    }
    vector_clear(&vec);
    struct vector_status* status = &vector_status(&vec);
    printf("\nsize: %llu capacity: %llu\n", status->size, status->capacity);
    free_vector(&vec);
}

struct list {
    u64 size, itemSize;
    void* value;
    struct list* next;
    struct list* prev;
};

int main(void) {
    struct list l;
    l.size = 0;
    l.itemSize = sizeof(int);
    l.next = NULL;
    l.prev = NULL;
}

