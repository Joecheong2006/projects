#include "command.h"
#include <string.h>

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