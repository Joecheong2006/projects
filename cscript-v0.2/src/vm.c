#include "vm.h"
#include "object.h"
#include "object_carrier.h"
#include "primitive_data.h"
#include "tracing.h"
#include "ast_node.h"
#include "bytecode.h"
#include "core/log.h"
#include "core/assert.h"
#include "core/memory.h"
#include <stdio.h>
#include <string.h>

#define LOG(name, fmt, ...) LOG_DEBUG("\t%-30s" fmt, name, __VA_ARGS__)

void init_vm(vm* v) {
    START_PROFILING();
    v->ip = 0;
    v->code = make_vector(u8);
    init_environment(&v->env);
    END_PROFILING(__func__);
}

void free_vm(vm* v) {
    START_PROFILING();
    free_environment(&v->env);
    free_vector(v->code);
    END_PROFILING(__func__);
}

INLINE static error_info initvar(vm* v) {
    cstring name = vector_backn(v->env.bp, 0).val.string;
    primitive_data rhs = vector_backn(v->env.bp, 1);
    vector_popn(v->env.bp, 2);
    if (rhs.type == PrimitiveDataTypeObjPtr) {
        if (rhs.val.carrier == NULL) {
            env_push_object(&v->env, make_object_carrier(name, NULL));
            LOG("initvar", "%s null\n", name);
            return (error_info){ .msg = NULL };
        }
        if (rhs.val.carrier->level != get_env_level(&v->env)) {
            rhs.val.carrier->name = name;
            env_push_object(&v->env, rhs.val.carrier);
            LOG("catach ret", "%s\n", name);
            return (error_info){ .msg = NULL };
        }
        if (rhs.val.carrier->obj->type != ObjectTypePrimitiveData) {
            object* obj = make_object_ref();
            object_ref* ref = get_object_true_type(obj);
            ref->ref_obj = rhs.val.carrier->obj;
            ref->ref_obj->ref_count++;
            return (error_info){ .msg = NULL };
        }

        object* obj = make_object_primitive_data();
        object_primitive_data* o = get_object_true_type(obj);
        object_primitive_data* rhs_obj = get_object_true_type(rhs.val.carrier->obj);
        o->val = rhs_obj->val;
        env_push_object(&v->env, make_object_carrier(name, obj));
        LOG("initvar", "%s\n", name);
        return (error_info){ .msg = NULL };
    }

    object* obj = make_object_primitive_data();
    object_primitive_data* o = get_object_true_type(obj);
    o->val = rhs;
    env_push_object(&v->env, make_object_carrier(name, obj));
    LOG("initvar", "%s\n", name);
    return (error_info){ .msg = NULL };
}

INLINE static error_info assign(vm* v) {
    object_carrier* carrier = vector_backn(v->env.bp, 0).val.carrier;
    primitive_data* rhs = &vector_backn(v->env.bp, 1);
    vector_popn(v->env.bp, 2);
    if (rhs->type == PrimitiveDataTypeObjPtr && (rhs->val.carrier == NULL || rhs->val.carrier->obj == NULL)) {
        if (carrier->obj) {
            carrier->obj->destroy(carrier->obj);
            carrier->obj = NULL;
        }
        LOG("assign", "%s\n", "null");
        return (error_info){ .msg = NULL };
    }

    if (carrier->obj == NULL) {
        if (rhs->type < PrimitiveDataTypeObjPtr) {
            carrier->obj = make_object_primitive_data();
            object_primitive_data* o = get_object_true_type(carrier->obj);
            o->val = *rhs;
            LOG("assign", "%s\t", carrier->name);
            print_primitive_data(&o->val);
        }
        else if (rhs->val.carrier->obj->type == ObjectTypePrimitiveData) {
            carrier->obj = make_object_primitive_data();
            object_primitive_data* o_rhs = get_object_true_type(rhs->val.carrier->obj);
            object_primitive_data* o = get_object_true_type(carrier->obj);
            o->val = o_rhs->val;
            LOG("assign", "%s\t", carrier->name);
            print_primitive_data(&o->val);
        }
        else {
            return (error_info){ .msg = "not implement object assignment for none beside primitive date" };
        }
        return (error_info){ .msg = NULL };
    }
    else if (carrier->obj->type == ObjectTypePrimitiveData) {
        if (rhs->type < PrimitiveDataTypeObjPtr) {
            object_primitive_data* o = get_object_true_type(carrier->obj);
            o->val = *rhs;
            LOG("assign", "%s\t", carrier->name);
            print_primitive_data(&o->val);
        }
        else if (rhs->val.carrier->obj->type == ObjectTypePrimitiveData) {
            object_primitive_data* o_rhs = get_object_true_type(rhs->val.carrier->obj);
            object_primitive_data* o = get_object_true_type(carrier->obj);
            o->val = o_rhs->val;
            LOG("assign", "%s\t", carrier->name);
            print_primitive_data(&o->val);
        }
        else {
            return (error_info){ .msg = "not implement object assignment for primitive data beside primitive date and none" };
        }
        return (error_info){ .msg = NULL };
    }
    else if (carrier->obj->type == ObjectTypeFunctionDef) {
        return (error_info){ .msg = "function cannot be assigned" };
    }
    else {
        ASSERT_MSG(0, "not implement object assignment");
    }

    return (error_info){ .msg = NULL };
}

INLINE static error_info funcdef(vm* v) {
    i8 param_count = vector_backn(v->env.bp, 0).val.int8;
    cstring name = vector_backn(v->env.bp, 1 + param_count).val.string;
    u32 end_pos = *(u32*)(v->code + v->ip + 1);
    v->ip += 4;

    object_carrier* carrier = env_find_object(&v->env, name);
    if (carrier && carrier->level == get_env_level(&v->env)) {
        return (error_info){ .msg = "redefine name" };
    }

    vector_popn(v->env.bp, 1);
    object* obj = make_object_function_def();
    object_function_def* def = get_object_true_type(obj);
    def->entry_point = v->ip;

    LOG("funcdef", "%s %d %d %d ", name, param_count, def->entry_point, end_pos);
    for (i32 i = 0; i < param_count; i++) {
        vector_push(def->args, vector_backn(v->env.bp, i).val.string);
        LOG_DEBUG_MSG("%s ", def->args[i]);
    }
    LOG_DEBUG_MSG("\n");
    vector_popn(v->env.bp, param_count + 1);

    env_push_object(&v->env, make_object_carrier(name, obj));

    v->ip = end_pos;

    return (error_info){ .msg = NULL };
}

INLINE static error_info funcall(vm* v) {
    i8 args_count = vector_backn(v->env.bp, 0).val.int8;
    object_carrier* carrier = vector_backn(v->env.bp, 1 + args_count).val.carrier;
    if (carrier->obj->type != ObjectTypeFunctionDef) {
        return (error_info){ .msg = "attempt to call a non funcation object" };
    }
    object_function_def* def = get_object_true_type(carrier->obj);

    if (args_count > (i8)vector_size(def->args)) {
        return (error_info){ .msg = "too many arguments" };
    }
    else if (args_count < (i8)vector_size(def->args)) {
        return (error_info){ .msg = "missing arguments" };
    }

    LOG("funcall", "%s %d %d\n", carrier->name, args_count, def->entry_point);
    vector_pop(v->env.bp);

    env_push_scope(&v->env);
    for (i32 i = 0; i < args_count; i++) {
        primitive_data data = {
            .val.string = def->args[i],
            .type = PrimitiveDataTypeString,
        };
        vector_push(v->env.bp, data);
        error_info ei = initvar(v);
        if (ei.msg) {
            return ei;
        }
    }
    vector_popn(v->env.bp, 1);

    primitive_data data = {
        .val.int64 = (i64)v->ip << 32 | (i64)(get_env_level(&v->env) - 1),
        .type = PrimitiveDataTypeInt64,
    };
    vector_push(v->env.bp, data);
    v->ip = def->entry_point;
    return (error_info){ .msg = NULL };
}

INLINE static error_info funcend(vm* v) {
    i32 org_ip = (i32)(vector_backn(v->env.bp, 0).val.int64 >> 32);
    i32 scope_level = (i32)((vector_backn(v->env.bp, 0).val.int64 << 32) >> 32);
    vector_pop(v->env.bp);
    vector_push(v->env.bp, (primitive_data){ .type = PrimitiveDataTypeObjPtr, .val.carrier = NULL });
    LOG("funcend", "%d %d %d\n", org_ip, scope_level, get_env_level(&v->env));
    do { 
        env_pop_scope(&v->env);
    } while (scope_level != get_env_level(&v->env));
    v->ip = org_ip;
    return (error_info){ .msg = NULL };
}

INLINE static error_info func_return(vm* v) {
    primitive_data data = vector_back(v->env.bp);
    vector_pop(v->env.bp);
    i32 org_ip = (i32)(vector_backn(v->env.bp, 0).val.int64 >> 32);
    i32 scope_level = (i32)((vector_backn(v->env.bp, 0).val.int64 << 32) >> 32);
    vector_pop(v->env.bp);

    if (data.type == PrimitiveDataTypeObjPtr) {
        LOG("return", "%d %d %d\n", org_ip, scope_level, get_env_level(&v->env));
        if (get_env_level(&v->env) == data.val.carrier->level) {
            env_remove_object_from_scope(&v->env, data.val.carrier);
            data.val.carrier->level++;
        }
        do { 
            env_pop_scope(&v->env);
        } while (scope_level != get_env_level(&v->env));
        v->ip = org_ip;
        vector_push(v->env.bp, (primitive_data){
                .val.carrier = data.val.carrier,
                .type = PrimitiveDataTypeObjPtr
            });
        return (error_info){ .msg = NULL };
    }
    else {
        vector_push(v->env.bp, data);
    }

    LOG("return", "%d %d %d\n", org_ip, scope_level, get_env_level(&v->env));
    do { 
        env_pop_scope(&v->env);
    } while (scope_level != get_env_level(&v->env));
    v->ip = org_ip;
    return (error_info){ .msg = NULL };
}

#define IMPL_ASSIGN(assign_name)\
        static char msg[54];\
        object_carrier* carrier = vector_backn(v->env.bp, 0).val.carrier;\
        primitive_data* rhs = &vector_backn(v->env.bp, 1);\
        vector_popn(v->env.bp, 2);\
        if (rhs->type == PrimitiveDataTypeObjPtr) {\
            if (rhs->val.carrier == NULL || rhs->val.carrier->obj == NULL) {\
                sprintf(msg, "attempt to perform " #assign_name " operation on none");\
                return (error_info){ .msg = msg };\
            }\
            if (rhs->val.carrier->obj->type != ObjectTypePrimitiveData) {\
                sprintf(msg, "attempt to perform " #assign_name " operation on %s", primitive_type_name[rhs->type]);\
                return (error_info){ .msg = msg };\
            }\
            object_primitive_data* o = get_object_true_type(rhs->val.carrier->obj);\
            *rhs = o->val;\
        }\
        if (carrier->obj->type != ObjectTypePrimitiveData) {\
            vector_popn(v->env.bp, 2);\
            sprintf(msg, "attempt to perform " #assign_name " operation on %s", primitive_type_name[carrier->obj->type]);\
            return (error_info){ .msg = msg};\
        }\
        object_primitive_data* o = get_object_true_type(carrier->obj);\
        error_info ei = primitive_data_##assign_name##_assign(&o->val, rhs);\
        if (ei.msg) {\
            return ei;\
        }\
        LOG(#assign_name "_assign", "%s\t", carrier->name);\
        print_primitive_data(&o->val);\
        break;

#define IMPL_ARITHMETIC(name)\
        static char msg[52];\
        u32 end = vector_size(v->env.bp);\
        primitive_data data;\
        primitive_data* lhs = v->env.bp + end - 2;\
        primitive_data* rhs = v->env.bp + end - 1;\
        vector_popn(v->env.bp, 2);\
        if (lhs->type == PrimitiveDataTypeObjPtr) {\
            if (lhs->val.carrier == NULL || lhs->val.carrier->obj == NULL) {\
                sprintf(msg, "attempt to " #name " with none");\
                return (error_info){ .msg = msg };\
            }\
            if (lhs->val.carrier->obj->type != ObjectTypePrimitiveData) {\
                sprintf(msg, "attempt to " #name " with a %s object", primitive_type_name[lhs->type]);\
                return (error_info){ .msg = msg };\
            }\
            object_primitive_data* o = get_object_true_type(lhs->val.carrier->obj);\
            *lhs = o->val;\
        }\
        if (rhs->type == PrimitiveDataTypeObjPtr) {\
            if (rhs->val.carrier == NULL || rhs->val.carrier->obj == NULL) {\
                sprintf(msg, "attempt to " #name " with none");\
                return (error_info){ .msg = msg };\
            }\
            if (rhs->val.carrier->obj->type != ObjectTypePrimitiveData) {\
                sprintf(msg, "attempt to " #name " with a %s object", primitive_type_name[rhs->type]);\
                return (error_info){ .msg = msg };\
            }\
            object_primitive_data* o = get_object_true_type(rhs->val.carrier->obj);\
            *rhs = o->val;\
        }\
        error_info ei = primitive_data_##name(&data, lhs, rhs);\
        if (ei.msg) {\
            return ei;\
        }\
        vector_push(v->env.bp, data);\
        LOG(#name, "", "");\
        print_primitive_data(&data);

#define IMPL_PUSH_CONST(flag, type_name, val_name, size)\
        primitive_data data = {\
            .type = PrimitiveDataType##flag,\
            .val.val_name = *(type_name*)(v->code + v->ip + 1)\
        };\
        vector_push(v->env.bp, data);\
        v->ip += size;\
        LOG("push", "", "");\
        print_primitive_data(&data);\
        break;

static error_info run(vm* v) {
    u32 size = vector_size(v->code);
    while(size > v->ip) {
        u8 code = v->code[v->ip];
        switch (code) {
        case ByteCodePushConst: {
            primitive_data data = { .type = v->code[v->ip + 1] };
            memcpy(&data.val, &v->code[v->ip + 2], primitive_size_map[data.type]);
            LOG("push", "", "");
            print_primitive_data(&data);
            vector_push(v->env.bp, data);
            v->ip += primitive_size_map[data.type] + 1;
            break;
        }
        case ByteCodePushUInt8: { IMPL_PUSH_CONST(UInt8, u8, uint8, 1) }
        case ByteCodePushInt8: { IMPL_PUSH_CONST(Int8, i8, int8, 1) }
        case ByteCodePushUInt16: { IMPL_PUSH_CONST(UInt16, u16, uint16, 2) }
        case ByteCodePushInt16: { IMPL_PUSH_CONST(Int16, i16, int16, 2) }
        case ByteCodePushUInt32: { IMPL_PUSH_CONST(UInt32, u32, uint32, 4) }
        case ByteCodePushInt32: { IMPL_PUSH_CONST(Int32, i32, int32, 4) }
        case ByteCodePushUInt64: { IMPL_PUSH_CONST(UInt64, u64, uint64, 8) }
        case ByteCodePushInt64: { IMPL_PUSH_CONST(Int64, i64, int64, 8) }
        case ByteCodePushFloat32: { IMPL_PUSH_CONST(Float32, f32, float32, 4) }
        case ByteCodePushFloat64: { IMPL_PUSH_CONST(Float64, f64, float64, 8) }
        case ByteCodePushNull: {
            primitive_data data = { .type = PrimitiveDataTypeObjPtr, .val.carrier = NULL };
            vector_push(v->env.bp, data);
            break;
        }
        case ByteCodePushTrue: {
            primitive_data data = { .type = PrimitiveDataTypeBoolean, .val.boolean = 1 };
            vector_push(v->env.bp, data);
            break;
        }
        case ByteCodePushFalse: {
            primitive_data data = { .type = PrimitiveDataTypeBoolean, .val.boolean = 0 };
            vector_push(v->env.bp, data);
            break;
        }
        case ByteCodeAdd: { IMPL_ARITHMETIC(add) break; }
        case ByteCodeSub: { IMPL_ARITHMETIC(sub) break; }
        case ByteCodeMul: { IMPL_ARITHMETIC(mul) break; }
        case ByteCodeDiv: { IMPL_ARITHMETIC(div) break; }
        case ByteCodeMod: { IMPL_ARITHMETIC(mod) break; }
        case ByteCodeEqual: { IMPL_ARITHMETIC(cmp_equal) break; }
        case ByteCodeNotEqual: { IMPL_ARITHMETIC(cmp_not_equal) break; }
        case ByteCodeGreaterThan: { IMPL_ARITHMETIC(cmp_greater_than) break; }
        case ByteCodeLessThan: { IMPL_ARITHMETIC(cmp_less_than) break; }
        case ByteCodeGreaterThanEqual: { IMPL_ARITHMETIC(cmp_greater_than_equal) break; }
        case ByteCodeLessThanEqual: { IMPL_ARITHMETIC(cmp_less_than_equal) break; }
        case ByteCodeAnd: { IMPL_ARITHMETIC(and) break; }
        case ByteCodeOr: { IMPL_ARITHMETIC(or) break; }
        case ByteCodeNegate: {
            primitive_data data;
            error_info ei = primitive_data_neg(&data, &vector_back(v->env.bp));
            if (ei.msg) {
                return ei;
            }
            vector_pop(v->env.bp);
            vector_push(v->env.bp, data);
            LOG("neg", "\n", "");
            print_primitive_data(&data);
            break;
        }
        case ByteCodeInitVar: {
            error_info ei = initvar(v);
            if (ei.msg) {
                return ei;
            }
            break;
        }
        case ByteCodePop: {
            primitive_data* data = &vector_back(v->env.bp);
            if (data->type == PrimitiveDataTypeObjPtr && data->val.carrier && data->val.carrier->level != get_env_level(&v->env)) {
                data->val.carrier->obj->destroy(data->val.carrier->obj);
                free_mem(data->val.carrier);
            }
            LOG("pop", "\n", "");
            vector_pop(v->env.bp);
            break;
        }
        case ByteCodePushName: {
            primitive_data data = {
                .val.string = *((cstring*)&v->code[v->ip + 1]),
                .type = PrimitiveDataTypeString,
            };
            vector_push(v->env.bp, data);
            LOG("push", "%s\n", data.val.string);
            v->ip += 8;
            break;
        }
        case ByteCodeRefIden: {
            cstring name = *((cstring*)&v->code[v->ip + 1]);
            object_carrier* carrier = env_find_object(&v->env, name);
            if (!carrier) {
                return (error_info){ .msg = "not found object '%s'" };
            }
            primitive_data data = {
                .val.carrier = carrier,
                .type = PrimitiveDataTypeObjPtr,
            };
            vector_push(v->env.bp, data);
            LOG("ref", "%s\n", name);
            v->ip += 8;
            break;
        }
        case ByteCodeAccessIden: {
            cstring name = *((cstring*)&v->code[v->ip + 1]);
            (void)name;
            LOG("access", "%s\n", name);
            v->ip += 8;
            break;
        }
        case ByteCodeAssign: {
            error_info ei = assign(v);
            if (ei.msg) {
                return ei;
            }
            break;
        }
        case ByteCodeAddAssign: { IMPL_ASSIGN(add) break; }
        case ByteCodeSubAssign: { IMPL_ASSIGN(sub) break; }
        case ByteCodeMulAssign: { IMPL_ASSIGN(mul) break; }
        case ByteCodeDivAssign: { IMPL_ASSIGN(div) break; }
        case ByteCodeModAssign: { IMPL_ASSIGN(mod) break; }
        case ByteCodeFuncDef: {
            error_info ei = funcdef(v);
            if (ei.msg) {
                return ei;
            }
            break;
        }
        case ByteCodeReturnNone:
        case ByteCodeFuncEnd: {
            error_info ei = funcend(v);
            if (ei.msg) {
                return ei;
            }
            break;
        }
        case ByteCodeFuncall: {
            error_info ei = funcall(v);
            if (ei.msg) {
                return ei;
            }
            break;
        }
        case ByteCodeReturn: {
            error_info ei = func_return(v);
            if (ei.msg) {
                return ei;
            }
            break;
        }
        case ByteCodeCountNewLine: {
            static i32 line_count = 0;
            LOG_DEBUG("\tline count: %d\n", ++line_count);
            break;
        }
        default: {
            LOG_ERROR("\tinvalid bytecode %d\n", code);
            ASSERT_MSG(0, "invalid bytecode");
            return (error_info){ .msg = "undefine bytecode" };
        }
        }
        v->ip++;
    }
    return (error_info){ .msg = NULL };
}

void vm_gen_bytecode(vm* v, ast_node** ast) {
    START_PROFILING();
    for_vector(ast, i, 0) {
        ast[i]->gen_bytecode(ast[i], v);
    }
    END_PROFILING(__func__);
}

error_info vm_run(vm* v) {
    env_push_scope(&v->env);
    error_info ei = run(v);
    env_pop_scope(&v->env);
    return ei;
}

