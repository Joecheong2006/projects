#include "stimer.h"

void start_stimer(stimer* t) {
    t->begin = clock();
}

void end_stimer(stimer* t) {
    t->dur = (f64)(clock() - t->begin) / CLOCKS_PER_SEC;
}
