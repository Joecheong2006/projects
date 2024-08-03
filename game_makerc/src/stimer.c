#include "stimer.h"
#include <stdio.h>

void start_stimer(stimer* t) {
    ASSERT_MSG(t != NULL, "invalid stimer");
    t->begin = clock();
}

void end_stimer(stimer* t) {
    ASSERT_MSG(t != NULL, "invalid stimer");
    t->dur = (f64)(clock() - t->begin) / CLOCKS_PER_SEC;
}
