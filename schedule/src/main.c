#include "memallocate.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // chdir
#include "command_line.h"

void print_menu(void) {}

error_type show_command_callback(int arg_index, char** argv, workspace* ws) {
	char* input = argv[arg_index - 1];
	char* end;
	i8 index = strtol(input, &end, 10);
	if (end == input) {
		printf("order must be a number\n");
		return ErrorInvalidParam;
	}
	if (index >= ws->lists_total) {
		printf("invalid order\n");
		return ErrorInvalidParam;
	}
	log_todo_list(ws->lists + index);
	return ErrorNone;
}

error_type show_nonarg_command_callback(int arg_index, char** argv, workspace* ws) {
	(void)arg_index, (void)argv;
	for_vector(ws->lists, i, 0) {
		log_todo_list(ws->lists + i);
	}
	return ErrorNone;
}

error_type add_menu_callback() {
	printf("add menu has not implemented yet\n");
	return ErrorNone;
}

error_type add_command_callback(int arg_index, char** argv, workspace* ws) {
	char* input = argv[arg_index - 1];
	todo_list tl;
	error_type et = init_todo_list(&tl, input);
	if (et != ErrorNone) {
		return et;
	}
	return workspace_add_list(ws, &tl);
}

error_type add_task_command_callback(int arg_index, char** argv, workspace* ws) {
	char* input_list_order = argv[arg_index - 2];
	char* end;
	i8 list_order = strtol(input_list_order, &end, 10);
	if (end == input_list_order) {
		printf("order must be a number\n");
		return ErrorInvalidParam;
	}

	char* input_text = argv[arg_index - 1];
	if (strlen(input_text) > TASK_NAME_MAX_LEN) {
		printf("invalid name size");
		return ErrorInvalidStringSize;
	}

	task t;
	strcpy(t.name, input_text);
	return workspace_add_task(ws, list_order, &t);
}

error_type remove_menu_callback(int arg_index, char** argv, workspace* ws) {
	printf("remove menu has not implemented yet\n");
	return ErrorNone;
}

error_type remove_todo_list_callback(int arg_index, char** argv, workspace* ws) {
	char* input_list_order = argv[arg_index - 1];
	char* end;
	i8 list_order = strtol(input_list_order, &end, 10);
	if (end == input_list_order) {
		printf("order must be a number\n");
		return ErrorInvalidParam;
	}
	return workspace_remove_list(ws, list_order);
}

error_type remove_task_callback(int arg_index, char** argv, workspace* ws) {
	char* input_list_order = argv[arg_index - 2];
	char* end;
	i8 list_order = strtol(input_list_order, &end, 10);
	if (end == input_list_order) {
		printf("list order must be a number\n");
		return ErrorInvalidParam;
	}
	char* input_task_order = argv[arg_index - 1];
	i8 task_order = strtol(input_task_order, &end, 10);
	if (end == input_task_order) {
		printf("task order must be a number\n");
		return ErrorInvalidParam;
	}

	return workspace_remove_task(ws, list_order, task_order);
}

int main(int argc, char** argv) {
	if (argc == 1) {
		print_menu();
		exit(0);
	}

	error_type et = ErrorNone;
	char* root = ".";
	char* cw = "daily";

	chdir(root);

	workspace ws;
	et = init_workspace(&ws, cw);
	et = workspace_init_list(&ws);

	arg_node node_show = init_arg_node(ArgTypeCommand, "show", show_nonarg_command_callback);
	arg_node node_switch = init_arg_node(ArgTypeCommand, "switch", NULL);
	arg_node node_add = init_arg_node(ArgTypeCommand, "add", add_menu_callback);
	arg_node node_remove = init_arg_node(ArgTypeCommand, "remove", remove_menu_callback);

	arg_node show_input_node = init_arg_node(ArgTypeInput, NULL, show_command_callback);
	arg_node_add_node(&node_show, &show_input_node);

	arg_node add_input_node = init_arg_node(ArgTypeInput, NULL, add_command_callback);
	arg_node_add_node(&node_add, &add_input_node);
	arg_node add_second_input_node = init_arg_node(ArgTypeInput, NULL, add_task_command_callback);
	arg_node_add_node(&add_input_node, &add_second_input_node);

	arg_node remove_input_node = init_arg_node(ArgTypeInput, NULL, remove_todo_list_callback);
	arg_node_add_node(&node_remove, &remove_input_node);
	arg_node remove_second_input_node = init_arg_node(ArgTypeInput, NULL, remove_task_callback);
	arg_node_add_node(&remove_input_node, &remove_second_input_node);

	arg_node node_start = init_arg_node(ArgTypeCommand, "", NULL);
	arg_node_add_node(&node_start, &node_show);
	arg_node_add_node(&node_start, &node_switch);
	arg_node_add_node(&node_start, &node_add);
	arg_node_add_node(&node_start, &node_remove);

	arg_node** cur = node_start.nodes;
	et = execute_command_line(cur, argc, argv, &ws);
	printf("error = %d\n", et);

	free_workspace(&ws);
	CHECK_MEMORY_LEAK();
}
