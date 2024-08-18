#ifndef _OBJECT_H_
#define _OBJECT_H_
#include "container/string.h"

typedef enum {
    ObjectTypeNone,
    ObjectTypeBool,
    ObjectTypeInt,
    ObjectTypeFloat,
    ObjectTypeString,
    ObjectTypeFunction,
    ObjectTypeArray,
    ObjectTypeUserType,
} ObjectType;

typedef struct object object;
struct object {
    ObjectType type;
    const char* name;
    void(*destroy)(object*);
};

object* make_object(ObjectType type, const char* name, u64 type_size, void(*destroy)(object*));
void free_object(object* obj);

void* get_object_true_type(object* obj);

typedef struct {
    i64 val;
} object_int;

void object_int_destroy(object* obj);
object* make_object_int(const char* name);

typedef struct {
    f64 val;
} object_float;

void object_float_destroy(object* obj);
object* make_object_float(const char* name);

typedef struct {
    f64 val;
} object_string;

void object_string_destroy(object* obj);
object* make_object_string(const char* name);

struct command;
typedef struct {
    vector(cstring) args;
    vector(struct command*) body;
} object_function;

void object_function_destroy(object* obj);
object* make_object_function(const char* name);

typedef struct {
    vector(object) members;
} object_user_type;

void object_user_type_destroy(object* obj);
object* make_object_user_type(const char* name);

#endif
