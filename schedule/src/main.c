#include "memallocate.h"
#include "workspace.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // chdir

#define ARG_NODE_MAX_LEN 4

typedef enum { ArgInputText, ArgInputNumber } arg_input_type;
typedef enum { ArgTypeLabel, ArgTypeInput, ArgTypeCommand } arg_type;
typedef error_type(*arg_callback)(int, char**, workspace*);
typedef struct arg_node arg_node;

struct arg_node {
	arg_type type;
	union {
		struct {
			arg_input_type type;
			union { char* text; int index; };
		} input;
		struct {
			char* name;
		} label;
		struct {
			arg_callback callback;
		} command;
	};
	arg_node* nodes[ARG_NODE_MAX_LEN];
};

arg_node init_arg_label_node(char* name) {
	arg_node result = (arg_node) {
		.type = ArgTypeLabel,
		.label.name = name
	};
	memset(&result.nodes, 0, sizeof(result.nodes));
	return result;
}

arg_node init_arg_input_node() {
	arg_node result = (arg_node) {
		.type = ArgTypeInput,
	};
	memset(&result.nodes, 0, sizeof(result.nodes));
	return result;
}

arg_node init_arg_command_node(arg_callback callback) {
	arg_node result = (arg_node) {
		.type = ArgTypeCommand,
		.command.callback = callback
	};
	memset(&result.nodes, 0, sizeof(result.nodes));
	return result;
}

void arg_node_add_node(arg_node* node, arg_node* new_node) {
	for (int i = 0; i < ARG_NODE_MAX_LEN; ++i) {
		if (node->nodes[i] == NULL) {
			node->nodes[i] = new_node;
			break;
		}
	}
}

void print_menu(void) {}
i32 is_number(char* str) {
	return str[0] >= '0' && str[0] <= '9';
}
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

	arg_node node_show = init_arg_label_node("show");
	arg_node node_switch = init_arg_label_node("switch");
	arg_node node_add = init_arg_label_node("add");

	arg_node node_start = init_arg_label_node("w");
	arg_node_add_node(&node_start, &node_show);
	arg_node_add_node(&node_start, &node_switch);
	arg_node_add_node(&node_start, &node_add);

	arg_node input_node1 = init_arg_input_node();
	arg_node_add_node(&node_show, &input_node1);

	arg_node** cur = node_start.nodes;

	for (int i = 0, arg_index = 1; i < ARG_NODE_MAX_LEN; ++i) {
		// if (arg_index >= argc) {
		// 	printf("end\n");
		// 	break;
		// }
		if (cur[i]->type == ArgTypeLabel) {
			if (strcmp(cur[i]->label.name, argv[arg_index]) == 0) {
				printf("%s ", cur[i]->label.name);
				arg_index++;
				if (arg_index >= argc) {
					printf("end\n");
					break;
				}
				cur = cur[i]->nodes;
				i = 0;
			}
			continue;
		}
		else if (cur[i]->type == ArgTypeCommand) {
			cur[i]->command.callback(argc, argv, &ws);
			cur = cur[i]->nodes;
			printf("command");
			i = 0;
			arg_index++;
			if (arg_index >= argc) {
				printf("end\n");
				break;
			}
			break;
		}
		char* end;
		i8 index = strtol(argv[arg_index], &end, 10);
		if (end != argv[arg_index]) {
			// load input
			cur = cur[i]->nodes;
			i = 0;
			printf("input ");
			arg_index++;
			if (arg_index >= argc) {
				printf("end\n");
				break;
			}
		}
		else {
			exit(ErrorInvalidParam);
			break;
		}
	}

	return 0;
	if (strcmp(argv[1], "show") == 0) {
		log_workspace(&ws);
	}
	if (strcmp(argv[1], "switch") == 0) {
		log_workspace(&ws);
	}
	else if (strcmp(argv[1], "add ")) {
		if (argc >= 3) {
			et = add_list(&ws, argv);
		}
		else {
		}
	}
	else if (is_number(argv[1])) {
		et = add_task(&ws, argv);
		if (strcmp(argv[2], "add") == 0) {
		}
		else if (strcmp(argv[2], "remove") == 0) {
		}
		else if (strcmp(argv[2], "rename") == 0) {
		}
		else if (strcmp(argv[2], "swap") == 0) {
		}
	}

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
