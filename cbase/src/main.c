#include <stdio.h>
#include "vector.h"
#include "util.h"


int main(void) {
    vector(int) a;
    init_vector(&a);
    vector_add(&a, 1);
    free_vector(&a);
}

