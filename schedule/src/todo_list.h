#ifndef SCHEDULE_H
#define SCHEDULE_H
#include "error.h"
#include "task.h"

#define TODO_LIST_NAME_MAX_LEN 50

typedef struct {
	char name[TODO_LIST_NAME_MAX_LEN];
	int tasks_total, tasks_finished;
	task* tasks;
} todo_list;

error_type make_todo_list(todo_list** result, char* name);
error_type free_todo_list(todo_list* sc);
error_type todo_list_init_task(todo_list* sc);
error_type todo_list_add_task(todo_list* tl, task* t);
error_type todo_list_get_task(todo_list* tl, task** result, char* name);
error_type todo_list_remove_task(todo_list* tl, char* name);

#endif