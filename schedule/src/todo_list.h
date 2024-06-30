#ifndef SCHEDULE_H
#define SCHEDULE_H
#include "error.h"
#include "task.h"
#include "vector.h"

#define TODO_LIST_NAME_MAX_LEN 50

typedef struct {
	char name[TODO_LIST_NAME_MAX_LEN];
	i8 tasks_total, tasks_finished, order;
	vector(task) tasks;
} todo_list;

error_type init_todo_list(todo_list* tl, char* name);
error_type free_todo_list(todo_list* tl);
error_type todo_list_init_task(todo_list* sc);
error_type todo_list_add_task(todo_list* tl, task* t);
error_type todo_list_remove_task(todo_list* tl, i8 order);
error_type todo_list_swap_task(todo_list* tl, i8 from_order, i8 to_order);
void log_todo_list(todo_list* tl);
error_type int_to_str(i8 i, char* str);

#endif