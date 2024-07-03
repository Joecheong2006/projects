#include "command.h"
#include <string.h>

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