#ifndef _OBJECT_H_
#define _OBJECT_H_
#include "container/string.h"
#include "primitive_data.h"

typedef enum {
    // ObjectTypeNone = PrimitiveDataTypeObjPtr + 1,
    ObjectTypeBool = PrimitiveDataTypeObjPtr + 1,
    ObjectTypePrimitiveData,
    ObjectTypeString,
    ObjectTypeFunctionDef,
    ObjectTypeArray,
    ObjectTypeRef,
    ObjectTypeUserType,
    ObjectErrorUndefine,
} ObjectType;

struct environment;
typedef struct object object;
struct object {
    ObjectType type;
    i32 ref_count;
    void(*destroy)(object*, struct environment*);
};

object* make_object(ObjectType type, u64 type_size, void(*destroy)(object*, struct environment*));
void* get_object_true_type(object* obj);

#define DEFINE_OBJECT_TYPE(type, body)\
    typedef struct { body } object_##type;\
    void object_##type_##destroy(object* obj, struct environment* inter);\
    object* make_object_##type(void);

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
        i32 entry_point;
)

DEFINE_OBJECT_TYPE(ref,
        object* ref_obj;
        // cstring ref_name;
)

DEFINE_OBJECT_TYPE(user_type,
        vector(object*) members;
)


#endif

