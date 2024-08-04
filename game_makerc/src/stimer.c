#include "stimer.h"
#include "core/assert.h"
#include "platform/platform.h"
#include <time.h>
#include <stdio.h>

void start_stimer(stimer* t) {
    ASSERT_MSG(t != NULL, "invalid stimer");
    t->begin = platform_get_time();
}

void end_stimer(stimer* t) {
    ASSERT_MSG(t != NULL, "invalid stimer");
    t->end = platform_get_time();
    t->dur = (f64)(t->end - t->begin);
}
