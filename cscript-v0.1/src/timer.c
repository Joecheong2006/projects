#include "timer.h"
#include "core/assert.h"
#include "platform/platform.h"

void start_timer(timer* t) {
    ASSERT_MSG(t != NULL, "invalid stimer");
    t->begin = platform_get_time();
}

void end_timer(timer* t) {
    ASSERT_MSG(t != NULL, "invalid stimer");
    t->end = platform_get_time();
    t->dur = (f64)(t->end - t->begin);
}
