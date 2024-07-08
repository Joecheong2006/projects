#ifndef ARG_H
#define ARG_H
#include "workspace.h"
#include "error.h"
#define ARG_NODE_MAX_LEN 6

typedef enum { ArgTypeInput, ArgTypeCommand } arg_type;
typedef error_type(*arg_callback)(int, char**, workspace*);
typedef struct arg_node arg_node;

struct arg_node {
	arg_type type;
	char* name;
	arg_callback callback;
	arg_node* nodes[ARG_NODE_MAX_LEN];
};

arg_node init_arg_node(arg_type type, char* name, arg_callback callback);
void arg_node_add_node(arg_node* node, arg_node* new_node);
error_type arg_node_call(arg_node* node, int arg_index, char** argv, workspace* ws);
error_type execute_command_line(arg_node** node, int argc, char** argv, void* in);

#endif
