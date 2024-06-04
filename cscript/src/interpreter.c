#include "interpreter.h"
#include "keys_define.h"
#include "environment.h"
#include "basic/memallocate.h"
#include "lexer.h"
#include "object.h"
#include "parser.h"

#include <string.h>
#include <stdlib.h>

#define IMPL_DATA_CHUNK_CONVERSION(conversion_name, operator)\
    void conversion_name##_data_chunk(void* out, data_chunk chunk) {\
        ASSERT_MSG(out != NULL, "invalid out");\
        switch (chunk.type) {\
        case NodeTypeChar: *(u8*)out operator chunk.val._char; break;\
        case NodeTypeInt: *(i64*)out operator chunk.val._int; break;\
        case NodeTypeFloat: *(f64*)out operator chunk.val._float; break;\
        default: ASSERT_MSG(false, "invalid conversion"); break;\
        }\
    }

void assign_data_chunk(void* out, data_chunk chunk) {
    ASSERT_MSG(out != NULL, "invalid out");
    switch (chunk.type) {
    case NodeTypeChar: *(u8*)out = chunk.val._char; break;
    case NodeTypeInt: *(i64*)out = chunk.val._int; break;
    case NodeTypeFloat: *(f64*)out = chunk.val._float; break;
    case NodeTypeString: *((char**)out) = chunk.val._string; break;
    case NodeTypeNull: break;
    default: ASSERT_MSG(false, "invalid conversion"); break;
    }
}

static IMPL_DATA_CHUNK_CONVERSION(negate, = -)
static IMPL_DATA_CHUNK_CONVERSION(plus_equal, +=)
static IMPL_DATA_CHUNK_CONVERSION(minus_equal, -=)
static IMPL_DATA_CHUNK_CONVERSION(multiply_equal, *=)
static IMPL_DATA_CHUNK_CONVERSION(division_equal, /=)

#define IMPL_DATA_CHUNK_ARITHMETIC(name, operation)\
    void name##_data_chunk(data_chunk* out, data_chunk* a, data_chunk* b) {\
        ASSERT_MSG(out != NULL, "invalid out");\
        ASSERT_MSG(a != NULL, "invalid a");\
        ASSERT_MSG(b != NULL, "invalid b");\
        out->type = a->type > b->type ? a->type : b->type;\
        if (out->type == NodeTypeNull) {\
            ASSERT_MSG(false, "invalid arithmetic to null");\
            exit(1);\
        }\
        type_cast(a, out->type);\
        type_cast(b, out->type);\
        switch (out->type) {\
            case NodeTypeInt: out->val._int = a->val._int operation b->val._int; break;\
            case NodeTypeChar: out->val._char = a->val._char operation b->val._char; break;\
            case NodeTypeFloat:out->val._float = a->val._float operation b->val._float; break;\
            default: ASSERT_MSG(false, "invalid arithmetic"); break;\
        }\
    }

void add_data_chunk(data_chunk* out, data_chunk* a, data_chunk* b) {
    ASSERT_MSG(out != NULL, "invalid out");
    ASSERT_MSG(a != NULL, "invalid a");
    ASSERT_MSG(b != NULL, "invalid b");
    out->type = a->type > b->type ? a->type : b->type;
    if (out->type == NodeTypeNull) {
        ASSERT_MSG(false, "invalid arithmetic to null");
        exit(1);
    }
    type_cast(a, out->type);
    type_cast(b, out->type);
    switch (out->type) {
        case NodeTypeInt: out->val._int = a->val._int + b->val._int; break;
        case NodeTypeChar: out->val._char = a->val._char + b->val._char; break;
        case NodeTypeFloat:out->val._float = a->val._float + b->val._float; break;
        case NodeTypeString: {
            out->val._string = MALLOC(strlen(a->val._string) + strlen(b->val._string) + 1);
            i32 i;
            for (i = 0; a->val._string[i] != 0; i++) {
                out->val._string[i] = a->val._string[i];
            }
            for (i32 j = i; b->val._string[i - j] != 0; i++) {
                out->val._string[i] = b->val._string[i - j];
            }
            out->val._string[i] = 0;
            FREE(a->val._string);
            FREE(b->val._string);
        } break;
        default: ASSERT_MSG(false, "invalid arithmetic"); break;
    }
}

static IMPL_DATA_CHUNK_ARITHMETIC(minus, -)
static IMPL_DATA_CHUNK_ARITHMETIC(multiply, *)
static IMPL_DATA_CHUNK_ARITHMETIC(division, /)

static void node_variable_impl(data_chunk* out, object* obj) {
    ASSERT_MSG(out != NULL, "invalid out");
    ASSERT_MSG(obj != NULL, "invalid obj");
    variable_info* info = obj->info;
    out->type = info->type;
    switch (out->type) {
        case NodeTypeInt: out->val._int = info->val._int; break;
        case NodeTypeChar: out->val._char = info->val._char; break;
        case NodeTypeFloat: out->val._float = info->val._float; break;
        case NodeTypeString: { 
            out->val._string = info->val._string; 
            info->val._string = NULL;
            info->type = NodeTypeNull;
        } break;
        case NodeTypeNull: break;
        default: ASSERT_MSG(false, "invalid out->type"); break;
    }
}

static void interpret_cal_expression(data_chunk* out, tree_node* node) {
    switch (node->type) {
    case NodeOperator: {
        data_chunk lhs, rhs;
        interpret_cal_expression(&lhs, node->nodes[0]);
        interpret_cal_expression(&rhs, node->nodes[1]);
        switch (node->object_type) {
        case OperatorPlus: add_data_chunk(out, &lhs, &rhs); break;
        case OperatorMinus: minus_data_chunk(out, &lhs, &rhs); break;
        case OperatorMultiply: multiply_data_chunk(out, &lhs, &rhs); break;
        case OperatorDivision: division_data_chunk(out, &lhs, &rhs); break;
        default: break;
        }
        } break;
    case NodeNegateOperator: {
        interpret_cal_expression(out, node->nodes[0]);
        negate_data_chunk(&out->val, *out);
    } break;
    case NodeDecNumber: {
        out->type = node->object_type;
        switch (node->object_type) {
        case NodeTypeInt: out->val._int = node->val._int; break;
        case NodeTypeFloat: out->val._float = node->val._float; break;
        default: break;
        }
    } break;
    case NodeHexNumber:
    case NodeOctNumber:
    case NodeBinNumber: {
        out->type = node->object_type;
        out->val._int = node->val._int;
    } break;
    case NodeCharLiteral: {
        out->type = node->object_type;
        out->val._char = node->name[0];
    } break;
    case NodeStringLiteral: {
        out->type = node->object_type;
        out->val._string = MALLOC(node->name_len + 1);
        memcpy(out->val._string, node->name, node->name_len);
        out->val._string[node->name_len] = 0;
    } break;
    case NodeVariable: {
        object* obj = get_object(node->name, node->name_len);
        if (!obj) {
            // NOTE: report run time error
            printf("not found name ");
            print_name(node->name, node->name_len);
            putchar('\n');
            exit(1);
        }
        if (obj->type != ObjectVariable) {
            printf("non variable object cannot be a part of expression\n");
            exit(1);
        }
        node_variable_impl(out, obj);
        break;
    } break;
    case NodeFunctionCall: {
        interpret(node);
        object* ret = vector_back(vector_back(env.scopes));
        if (ret->name[0] != '.') {
            out->type = NodeTypeNull;
            break;
        }
        if (ret->type != ObjectVariable) {
            printf("not implement expression type %d yet\n", ret->type);
            exit(1);
        }
        node_variable_impl(out, ret);
        free_object(ret);
        scope_pop(&vector_back(env.scopes));
    } break;
    default: ASSERT_MSG(false, "invalid out->type"); break;
    }
}

void type_cast(data_chunk* chunk, i32 type) {
    switch (type - chunk->type) {
    case NodeTypeInt - NodeTypeFloat: chunk->val._int = chunk->val._float; break;
    case NodeTypeInt - NodeTypeChar: chunk->val._int = chunk->val._char; break;
    case NodeTypeChar - NodeTypeFloat: chunk->val._char = chunk->val._float; break;
    case NodeTypeChar - NodeTypeInt: chunk->val._char = chunk->val._int; break;
    case NodeTypeFloat - NodeTypeInt: chunk->val._float = chunk->val._int; break;
    case NodeTypeFloat - NodeTypeChar: chunk->val._float = chunk->val._char; break;
    case NodeTypeString - NodeTypeChar: {
        char ch = chunk->val._char;
        chunk->val._string = MALLOC(sizeof(char) + 1);
        chunk->val._string[0] = ch;
        chunk->val._string[1] = 0;
    } break;
    default: break;
    }

    if (type == NodeTypeNull) {
        ASSERT_MSG(false, "invalid type cast null");
        exit(1);
    }

    chunk->type = type;
}

static void print_variable(object* obj) {
    variable_info* info = obj->info;
    switch (info->type) {
        case NodeTypeInt: printf("%s = %lld\n", obj->name, info->val._int); break;
        case NodeTypeFloat: printf("%s = %g\n", obj->name, info->val._float); break;
        case NodeTypeChar: printf("%s = %c\n", obj->name, info->val._char); break;
        case NodeTypeString: printf("%s = %s\n", obj->name, info->val._string); break;
        default: break;
    }
}

void interpret_initialize_rvalue_function(object* obj, tree_node* node) {
    tree_node* rvalue = node->nodes[0]->nodes[0];
    object* rvalue_obj= get_object(rvalue->name, rvalue->name_len);
    if (obj) {
        free_object(obj);
        obj = make_ref_object(rvalue_obj);
        obj->name = make_stringn(node->name, node->name_len);
        return;
    }
    obj = make_ref_object(rvalue_obj);
    obj->name = make_stringn(node->name, node->name_len);
    register_object(obj);
    return;
}

static void interpret_variable_initialize(tree_node* node) {
    object* obj = get_object(node->name, node->name_len);

    tree_node* rvalue = node->nodes[0]->nodes[0];
    if (rvalue->type == NodeVariable) {
        object* rvalue_obj= get_object(rvalue->name, rvalue->name_len);
        if (!rvalue_obj) {
            printf("not found name ");
            print_name(rvalue->name, rvalue->name_len);
            putchar('\n');
            exit(1);
        }
        if (rvalue_obj->type == ObjectFunction) {
            interpret_initialize_rvalue_function(obj, node);
            return;
        }
    }

    data_chunk rhs;
    interpret_cal_expression(&rhs, node->nodes[0]->nodes[0]);
    if (obj) {
        variable_info* var = obj->info;
        if (var->type != rhs.type) {
            free_object_variable(obj->info);
            obj->info = make_variable_info(&rhs.type);
        }
        else if (var->type == NodeTypeString && var->val._string != NULL) {
            FREE(var->val._string);
        }
    }
    else {
        obj = make_object(&(object){
                .name = make_stringn(node->name, node->name_len),
                .type = ObjectVariable,
                .info = make_variable_info(&rhs.type),
                });

        register_object(obj);
    }

    if (rhs.type == NodeTypeNull) {
        printf("%s = null\n", obj->name);
        return;
    }
    variable_info* var = obj->info;
    assign_data_chunk(&var->val, rhs);
    print_variable(obj);
}

static void interpret_variable_assignment(tree_node* node) {
    object* obj = get_object(node->name, node->name_len);
    if (!obj) {
        printf("not found object ");
        print_name(node->name, node->name_len);
        putchar('\n');
        exit(1);
    }

    variable_info* info = obj->info;
    data_chunk chunk;
    interpret_cal_expression(&chunk, node->nodes[0]->nodes[0]);
    type_cast(&chunk, info->type);

    switch (node->nodes[0]->object_type) {
    case OperatorPlusEqual: plus_equal_data_chunk(&info->val, chunk); break;
    case OperatorMinusEqual: minus_equal_data_chunk(&info->val, chunk); break;
    case OperatorMultiplyEqual: multiply_equal_data_chunk(&info->val, chunk); break;
    case OperatorDivisionEqual: division_equal_data_chunk(&info->val, chunk); break;
    default: break;
    }

    print_variable(obj);
}

static void interpret_function_decl(tree_node* node) {
    object* obj = get_object(node->name, node->name_len);
    if (obj) {
        // NOTE(error): function already declared
        exit(1);
    }
    obj = make_object(&(object){
            .name = make_stringn(node->name, node->name_len),
            .type = ObjectFunction,
            .info = make_function_info(node->nodes[0]),
            });

    register_object(obj);
}

static void interpret_function_call(tree_node* node) {
    object* fn_obj = get_object(node->name, node->name_len);
    if (!fn_obj) {
        printf("not found function ");
        print_name(node->name, node->name_len);
        putchar('\n');
        exit(1);
    }
    if (fn_obj->type != ObjectFunction) {
        print_name(node->name, node->name_len);
        printf(" is not function\n");
        exit(1);
    }
    function_info* fn_info = fn_obj->info;
    tree_node* params = node->nodes[0];
    if (vector_size(params->nodes) != vector_size(fn_info->params)) {
        printf("doesn't match %s's parameters\n", fn_obj->name);
        exit(1);
    }

    vector_push(env.scopes, make_scope());

    for_vector(fn_info->params, i, 0) {
        if (params->nodes[i]->type == NodeVariable) {
            object* obj = get_object(params->nodes[i]->name, params->nodes[i]->name_len);
            if (!obj) {
                printf("not fount variable name\n");
                exit(1);
            }
            if (obj->type == ObjectFunction) {
                object* param = make_ref_object(obj);
                param->name = make_string(fn_info->params[i]);
                register_object(param);
                continue;
            }
            // object* param = make_ref_object(obj);
            object* param = copy_object(obj);
            param->name = make_string(fn_info->params[i]);
            register_object(param);
            continue;

        }
        // else if (params->type == NodeFunctionCall) {
        //     object* obj = get_object(params->nodes[i]->name, params->nodes[i]->name_len);
        //     if (!obj) {
        //         printf("not fount variable name\n");
        //         exit(1);
        //     }
        //     param->name = make_string(fn_info->params[i]);
        //     register_object(param);
        //     continue;
        // }

        data_chunk chunk = { .type = -1 };
        interpret_cal_expression(&chunk, params->nodes[i]);
        object* param = make_object(&(object){
                .name = make_string(fn_info->params[i]),
                .type = ObjectVariable,
                .info = make_variable_info(&chunk.type),
                });
        register_object(param);
        variable_info* info = param->info;
        assign_data_chunk(&info->val, chunk);
    }

    for_vector(fn_info->body, i, 0) {
        interpret(fn_info->body[i]);
        if (fn_info->body[i]->type == NodeReturn || fn_info->body[i]->type == NodeEnd) {
            break;
        }
    }
}

static void interpret_end() {
    scope s = vector_back(env.scopes);
    for (i32 i = (i32)vector_size(s) - 1; i > -1; --i) {
        vector_pop(env.object_map.data[hash_object(s[i], env.object_map.size)]);
    }
    free_scope(&vector_back(env.scopes));
    vector_pop(env.scopes);
}

static void interpret_function_return(tree_node* ins) {
    data_chunk chunk = { .type = -1 };
    interpret_cal_expression(&chunk, ins->nodes[0]);
    object* ret_obj = make_object(&(object){
            .name = make_string(".ret"),
            .type = ObjectVariable,
            .info = make_variable_info(&chunk.type),
            });
    interpret_end();
    vector_push(vector_back(env.scopes), ret_obj);
    variable_info* info = ret_obj->info;
    assign_data_chunk(&info->val, chunk);
}

void interpret(tree_node* ins) {
    switch (ins->type) {
    case NodeVariableInitialize: interpret_variable_initialize(ins); break;
    case NodeVariableAssignment: interpret_variable_assignment(ins); break;
    case NodeFunctionDecl: interpret_function_decl(ins); break;
    case NodeFunctionCall: interpret_function_call(ins); break;
    case NodeReturn: interpret_function_return(ins); break;
    case NodeEnd: interpret_end(); break;
    default: printf("not implement node instruction %d yet\n", ins->type); break;
    }
}

