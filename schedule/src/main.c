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
	log_error(et, "schedule init task");
	
	// task t = { .name = "task1", };
	// et = todo_list_add_task(sc, &t);
	// log_error(et, "todo list add task");

	task* ret;
	et = todo_list_get_task(sc, &ret, "task1");
	log_error(et, "schedule found task");
	if (ret) {
		printf("found %s in order %d\n", ret->name, ret->order);
	}

	et = free_todo_list(sc);
	log_error(et, "free schedule");

	CHECK_MEMORY_LEAK();
}
