#include "memallocate.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // chdir
#include "command.h"

void print_menu(void) {}

error_type add_list(workspace* ws, char** argv) {
	error_type et;
	todo_list sc;
	if ((et = init_todo_list(&sc, argv[2])) != ErrorNone)
		return et;

	if ((et = workspace_add_list(ws, &sc)) != ErrorNone)
		return et;

	return ErrorNone;
}

error_type add_task(workspace* ws, char** argv) {
	char* end;
	i8 index = strtol(argv[1], &end, 10);
	if (end == argv[1])
		return ErrorInvalidParam;
	workspace_add_task(ws, index, NULL);
	return ErrorNone;
}

void log_arg(arg_node* node) {
	static int s = 0;
	s++;
	for (int i = 0; i < ARG_NODE_MAX_LEN; ++i) {
		if (node->nodes[i] != NULL) {
			printf("%d %d %d\n", s, i, node->nodes[i]->type);
			log_arg(node->nodes[i]);
			s--;
		}
	}
}

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

error_type add_command_callback(int arg_index, char** argv, workspace* ws) {
	char* input = argv[arg_index - 1];
	chdir(ws->name);
	todo_list tl;
	error_type et = init_todo_list(&tl, input);
	if (et != ErrorNone) {
		return et;
	}
	et = workspace_add_list(ws, &tl);
	chdir("..");
	return et;
}

error_type add_task_command_callback(int arg_index, char** argv, workspace* ws) {
	char* input_list_order = argv[arg_index - 2];
	char* end;
	i8 list_order = strtol(input_list_order, &end, 10);
	if (end == input_list_order) {
		printf("order must be a number\n");
		return ErrorInvalidParam;
	}

	if (list_order >= ws->lists_total || list_order < 0) {
		printf("invalid order\n");
		return ErrorInvalidParam;
	}

	char* input_text = argv[arg_index - 1];
	if (strlen(input_text) > TASK_NAME_MAX_LEN) {
		printf("invalid name size");
		return ErrorInvalidStringSize;
	}

	chdir(ws->name);
	task t;
	strcpy(t.name, input_text);
	error_type et = workspace_add_task(ws, list_order, &t);
	if (et != ErrorNone) {
		return et;
	}
	chdir("..");
	return ErrorNone;
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
	arg_node node_add = init_arg_node(ArgTypeCommand, "add", NULL);

	arg_node node_start = init_arg_node(ArgTypeCommand, "", NULL);
	arg_node_add_node(&node_start, &node_show);
	arg_node_add_node(&node_start, &node_switch);
	arg_node_add_node(&node_start, &node_add);

	arg_node show_input_node = init_arg_node(ArgTypeInput, NULL, show_command_callback);
	arg_node_add_node(&node_show, &show_input_node);
	arg_node add_input_node = init_arg_node(ArgTypeInput, NULL, add_command_callback);
	arg_node_add_node(&node_add, &add_input_node);
	arg_node add_second_input_node = init_arg_node(ArgTypeInput, NULL, add_task_command_callback);
	arg_node_add_node(&add_input_node, &add_second_input_node);

	arg_node** cur = node_start.nodes;
	for (int i = 0, arg_index = 1; i < ARG_NODE_MAX_LEN;) {
		if (cur[i] == NULL || arg_index > argc) {
			printf("invalid command\n");
			break;
		}
		if (cur[i]->type == ArgTypeCommand) {
			if (strcmp(cur[i]->name, argv[arg_index]) == 0) {
				if (++arg_index >= argc) {
					et = arg_node_call(cur[i], arg_index, argv, &ws);
					break;
				}
				cur = cur[i]->nodes;
				i = 0;
				continue;
			}
			i++;
			continue;
		}
		if (cur[i]->type == ArgTypeInput) {
			if (++arg_index >= argc) {
				et = arg_node_call(cur[i], arg_index, argv, &ws);
				break;
			}
			cur = cur[i]->nodes;
			i = 0;
			continue;
		}
		et = ErrorInvalidType;
		break;
	}
	printf("error = %d\n", et);

	free_workspace(&ws);
	CHECK_MEMORY_LEAK();

	return 0;
	// printf("name: '%s'\n", &ws.lists[0]);

	// chdir(ws.name);
	// task t = { .name = "task2", };
	// workspace_add_task(&ws, 1, &t);
	// et = todo_list_add_task(&ws.lists[0], &t);
	// log_error(et, "todo list add task");
	// chdir("..");

	todo_list_swap_task(&ws.lists[0], 1, 3);

}
