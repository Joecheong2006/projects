#include "command_line.h"
#include <string.h>
#include <stdio.h>

arg_node init_arg_node(arg_type type, char* name, arg_callback callback) {
	arg_node result = {
		.type = type,
		.name = name,
		.callback = callback,
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

error_type arg_node_call(arg_node* node, int arg_index, char** argv, workspace* ws) {
	if (node->callback == NULL) {
		return ErrorInvalidParam;
	}
	return node->callback(arg_index, argv, ws);
}

error_type execute_command_line(arg_node** node, int argc, char** argv, void* in) {
	error_type et = ErrorNone;
	for (int i = 0, arg_index = 1; i < ARG_NODE_MAX_LEN;) {
		if (node[i] == NULL || arg_index > argc) {
			printf("invalid command\n");
			break;
		}
		if (node[i]->type == ArgTypeCommand) {
			if (strcmp(node[i]->name, argv[arg_index]) == 0) {
				if (++arg_index >= argc) {
					et = arg_node_call(node[i], arg_index, argv, in);
					break;
				}
				node = node[i]->nodes;
				i = 0;
				continue;
			}
			i++;
			continue;
		}
		if (node[i]->type == ArgTypeInput) {
			if (++arg_index >= argc) {
				et = arg_node_call(node[i], arg_index, argv, in);
				break;
			}
			node = node[i]->nodes;
			i = 0;
			continue;
		}
		et = ErrorInvalidType;
		break;
	}
	return et;
}
