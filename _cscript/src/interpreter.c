#include "interpreter.h"
#include "keys_define.h"
#include "environment.h"
#include "basic/memallocate.h"
#include "lexer.h"
#include "object.h"
#include "parser.h"

#include <string.h>
#include <stdlib.h>

void interpret(tree_node* ins) {
    switch (ins->type) {
    default: printf("not implement node instruction %d yet\n", ins->type); break;
    }
}

