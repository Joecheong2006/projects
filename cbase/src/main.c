#include <stdio.h>
#include "vector.h"
#include "util.h"


int main(void) {
    vector(int) vec;
    init_vector(&vec);
    for (u64 i = 0; i < 31; ++i) {
        vector_add(&vec, i);
    }
    for (u64 i = 0; i < vector_status(&vec).size; ++i) {
        printf("%d ", vec[i]);
    }
    struct vector_status* status = &vector_status(&vec);
    printf("\nsize: %llu capacity: %llu\n", status->size, status->capacity);
    free_vector(&vec);
}

