#ifndef TASK_H
#define TASK_H
#include "util.h"

#define TASK_NAME_MAX_LEN 50

typedef struct {
	char name[TASK_NAME_MAX_LEN];
	u8 order, finished;
} task;

void log_task(task* t);

#endif
