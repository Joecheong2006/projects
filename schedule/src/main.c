#include "memallocate.h"
#include "todo_list.h"
#include <stdio.h>
#include <stdlib.h>

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
	todo_list* sc;
	et = make_todo_list(&sc, "learning");
	log_error(et, "make schedule");
	printf("name: '%s'\n", sc->name);

	et = todo_list_init_task(sc);
	log_error(et, "init task");
	log_todo_list(sc);
	todo_list_swap_task(sc, 1, 3);
	
	// task t = { .name = "task1", };
	// et = todo_list_add_task(sc, &t);
	// log_error(et, "todo list add task");

	et = free_todo_list(sc);
	log_error(et, "free todo_list");

	CHECK_MEMORY_LEAK();
}
