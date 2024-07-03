#ifndef ARG_H
#define ARG_H
#include "workspace.h"
#include "error.h"
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

arg_node init_arg_label_node(char* name);
arg_node init_arg_input_node();
arg_node init_arg_command_node(arg_callback callback);
void arg_node_add_node(arg_node* node, arg_node* new_node);


#endif
