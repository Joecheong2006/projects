#include "task.h"
#include <stdio.h>

void log_task(task* t) {
	printf("[%d] %s %c\n", t->order, t->name, t->finished ? 'O' : 'X');
}
