#ifndef _OBJECT_H_
#define _OBJECT_H_
#include "container/string.h"
#include "primitive_data.h"

typedef enum {
    ObjectTypeNone,
    ObjectTypeBool,
    ObjectTypePrimitiveData,
    ObjectTypeString,
    ObjectTypeFunctionDef,
    ObjectTypeArray,
    ObjectTypeRef,
    ObjectTypeUserType,
    ObjectErrorUndefine,
} ObjectType;

typedef struct object object;
struct object {
    ObjectType type;
    cstring name;
    i32 level, ref_count;
    void(*destroy)(object*);
};

object* make_object(ObjectType type, cstring name, u64 type_size, void(*destroy)(object*));
void* get_object_true_type(object* obj);

#define DEFINE_OBJECT_TYPE(type, body)\
    typedef struct { body } object_##type;\
    void object_##type_##destroy(object* obj);\
    object* make_object_##type(cstring name);

void object_none_destroy(object* obj);
object* make_object_none(cstring name);

DEFINE_OBJECT_TYPE(bool,
        u8 val;
)

DEFINE_OBJECT_TYPE(primitive_data, 
        primitive_data val;
)

DEFINE_OBJECT_TYPE(string,
        string val;
)

struct command;
DEFINE_OBJECT_TYPE(function_def,
        vector(cstring) args;
        vector(struct command*) body;
)

DEFINE_OBJECT_TYPE(ref,
        cstring ref_name;
)

DEFINE_OBJECT_TYPE(user_type,
        vector(object*) members;
)


#endif

