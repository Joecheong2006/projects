#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include "basic/hashmap.h"
#include "interpreter.h"
#include "object.h"

typedef enum {
    ParserErrorNoError,
    ParserErrorMissingToken,
    ParserErrorMissingLhs,
    ParserErrorMissingRhs,
    ParserErrorMissingOpenBracket,
    ParserErrorMissingCloseBracket,
    ParserErrorMissingOperator,
    ParserErrorMissingAssignOperator,
    ParserErrorMissingSeparator,
    ParserErrorExpectedExpression,
    ParserErrorInvalidOperandsType,
    ParserErrorUndefineName,
    RuntimeErrorUnkownName,
} ErrorType;

typedef struct {
    ErrorType type;
    i32 line, pos;
} error_message;

typedef vector(object*) scope;

void add_error_message(error_message message);

struct _environment {
    hashmap object_map;
    vector(scope) scopes;
    vector(error_message) error_messages;
    interpreter inter;
};

extern struct _environment env;

object* get_object(const char* name, u64 len);

void init_environment(void);
void delete_environment(void);

scope make_scope(void);
void scope_push(scope* s, object* obj);
void scope_pop(scope* s);
void free_scope(scope* s);

void construct_object(object* obj);
void destruct_object(object* obj);

#endif
