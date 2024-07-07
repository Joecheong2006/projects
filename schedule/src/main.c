#include "memallocate.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // chdir
#include "command_line.h"

void print_menu(void) {}

error_type list_list_command_callback(int arg_index, char** argv, workspace* ws) {
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

error_type list_all_command_callback(int arg_index, char** argv, workspace* ws) {
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
	error_type et = workspace_add_task(ws, list_order, &t);
	if (et == ErrorInvalidParam) {
		printf("invalid order %s\n", input_list_order);
	}
	return et;
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
	if (list_order >= ws->lists_total || list_order < 0) {
		printf("invalid list order %s\n", input_list_order);
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
	if (list_order >= ws->lists_total || list_order < 0) {
		printf("invalid list order %s\n", input_list_order);
	}
	else if (task_order >= ws->lists[list_order].tasks_total || task_order < 0) {
		printf("invalid task order %s\n", input_task_order);
	}
	return workspace_remove_task(ws, list_order, task_order);
}

error_type swap_menu_callback(int arg_index, char** argv, workspace* ws) {
	printf("swap menu has not implemented yet\n");
	return ErrorNone;
}

error_type swap_list_callback(int arg_index, char** argv, workspace* ws) {
	char* input_list_order_first = argv[arg_index - 2];
	char* end;
	i8 list_order_first = strtol(input_list_order_first, &end, 10);
	if (end == input_list_order_first) {
		printf("list order must be a number\n");
		return ErrorInvalidParam;
	}
	char* input_list_order_second = argv[arg_index - 1];
	i8 list_order_second = strtol(input_list_order_second, &end, 10);
	if (end == input_list_order_second) {
		printf("list order must be a number\n");
		return ErrorInvalidParam;
	}
	if (list_order_first  >= ws->lists_total || list_order_first < 0) {
		printf("invlid order first %s", input_list_order_first);
	}
	else if (list_order_second  >= ws->lists_total || list_order_second < 0) {
		printf("invlid order second %s", input_list_order_second);
	}
	return workspace_swap_list(ws, list_order_first, list_order_second);
}

error_type swap_task_callback(int arg_index, char** argv, workspace* ws) {
	char* input_task_order_first = argv[arg_index - 2];
	char* end;
	i8 task_order_first = strtol(input_task_order_first, &end, 10);
	if (end == input_task_order_first) {
		printf("list order must be a number\n");
		return ErrorInvalidParam;
	}
	char* input_task_order_second = argv[arg_index - 1];
	i8 task_order_second = strtol(input_task_order_second, &end, 10);
	if (end == input_task_order_second) {
		printf("list order must be a number\n");
		return ErrorInvalidParam;
	}
	char* input_list_order = argv[arg_index - 3];
	i8 list_order = strtol(input_list_order, &end, 10);
	if (end == input_list_order) {
		printf("list order must be a number\n");
		return ErrorInvalidParam;
	}

	if (list_order >= ws->lists_total || list_order < 0) {
		printf("invlid list order %s\n", input_list_order);
	}
	else if (task_order_first >= ws->lists[list_order].tasks_total || task_order_first < 0) {
		printf("invlid task order first %s\n", input_task_order_first);
	}
	else if (task_order_second >= ws->lists[list_order].tasks_total || task_order_second < 0) {
		printf("invlid task order second %s\n", input_task_order_second);
	}
	return workspace_swap_task(ws, list_order, task_order_first, task_order_second);
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

	arg_node node_show = init_arg_node(ArgTypeCommand, "list", list_all_command_callback);
	arg_node node_switch = init_arg_node(ArgTypeCommand, "switch", NULL);
	arg_node node_add = init_arg_node(ArgTypeCommand, "add", add_menu_callback);
	arg_node node_remove = init_arg_node(ArgTypeCommand, "remove", remove_menu_callback);
	arg_node node_swap = init_arg_node(ArgTypeCommand, "swap", swap_menu_callback);

	arg_node show_input_node = init_arg_node(ArgTypeInput, NULL, list_list_command_callback);
	arg_node_add_node(&node_show, &show_input_node);

	arg_node add_input_node = init_arg_node(ArgTypeInput, NULL, add_command_callback);
	arg_node_add_node(&node_add, &add_input_node);
	arg_node add_second_input_node = init_arg_node(ArgTypeInput, NULL, add_task_command_callback);
	arg_node_add_node(&add_input_node, &add_second_input_node);

	arg_node remove_input_node = init_arg_node(ArgTypeInput, NULL, remove_todo_list_callback);
	arg_node_add_node(&node_remove, &remove_input_node);
	arg_node remove_second_input_node = init_arg_node(ArgTypeInput, NULL, remove_task_callback);
	arg_node_add_node(&remove_input_node, &remove_second_input_node);

	arg_node swap_input_node = init_arg_node(ArgTypeInput, NULL, NULL);
	arg_node_add_node(&node_swap, &swap_input_node);
	arg_node swap_second_input_node = init_arg_node(ArgTypeInput, NULL, swap_list_callback);
	arg_node_add_node(&swap_input_node, &swap_second_input_node);
	arg_node swap_third_input_node = init_arg_node(ArgTypeInput, NULL, swap_task_callback);
	arg_node_add_node(&swap_second_input_node, &swap_third_input_node);

	arg_node node_start = init_arg_node(ArgTypeCommand, "", NULL);
	arg_node_add_node(&node_start, &node_show);
	arg_node_add_node(&node_start, &node_switch);
	arg_node_add_node(&node_start, &node_add);
	arg_node_add_node(&node_start, &node_remove);
	arg_node_add_node(&node_start, &node_swap);

	arg_node** cur = node_start.nodes;
	et = execute_command_line(cur, argc, argv, &ws);
	printf("error = %d\n", et);

	free_workspace(&ws);
	CHECK_MEMORY_LEAK();
}
