#include "memallocate.h"
#include "workspace.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // chdir

/*
	schedule manager: add remove change -> schedule
	task: add remove change -> self
		description -> string
		finish -> bool
		requirement -> strings
*/

void print_menu(void) {}

int main(int argc, char** argv) {
	if (argc == 0) {
		print_menu();
		exit(0);
	}

	error_type et = ErrorNone;

	workspace ws;
	et = init_workspace(&ws, "daily");
	log_error(et, "make workspace");

	et = workspace_init_list(&ws);
	log_error(et, "workspace list");
	log_workspace(&ws);

	// todo_list sc;
	// et = init_todo_list(&sc, "learning");
	// log_error(et, "make todo");
	// et = workspace_add_list(&ws, &sc);
	// log_error(et, "add list");

	// printf("name: '%s'\n", &ws.lists[0]);

	// chdir(ws.name);
	// task t = { .name = "task2", };
	// workspace_add_task(&ws, 1, &t);
	// et = todo_list_add_task(&ws.lists[0], &t);
	// log_error(et, "todo list add task");
	// chdir("..");

	todo_list_swap_task(&ws.lists[0], 1, 3);

	free_workspace(&ws);

	CHECK_MEMORY_LEAK();
}
