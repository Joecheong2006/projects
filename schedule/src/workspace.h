#ifndef WORKSAPCE_H
#define WORKSPACE_H
#include "todo_list.h"
#include "vector.h"

#define WORKSPACE_NAME_MAX_LEN 50

typedef struct {
	vector(todo_list) lists;
	char name[WORKSPACE_NAME_MAX_LEN];
	i8 lists_total;
} workspace;

error_type init_workspace(workspace* ws, char* name);
error_type free_workspace(workspace* ws);
error_type workspace_init_list(workspace* ws);
error_type workspace_add_list(workspace* ws, todo_list* tl);
error_type workspace_add_task(workspace* ws, i8 list_order, task* t);
error_type workspace_remove_list(workspace* ws, i8 order);
error_type workspace_remove_task(workspace* ws, i8 list_order, i8 task_order);
error_type workspace_swap_list(workspace* ws, i8 from_order, i8 to_order);
error_type workspace_swap_task(workspace* ws, i8 list_order, i8 from_order, i8 to_order);
void log_workspace(workspace* ws);

#endif
