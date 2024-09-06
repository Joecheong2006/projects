#include "vm.h"
#include "object.h"
#include "primitive_data.h"
#include "tracing.h"
#include "bytecode.h"
#include "core/log.h"
#include "core/assert.h"
#include <string.h>

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
    if (rhs.type >= ObjectTypeNone) {
        ASSERT_MSG(rhs.val.carrier->obj->type == ObjectTypePrimitiveData, "not implement initvar obj yet");
        object* obj = make_object_primitive_data(name);
        object_primitive_data* o = get_object_true_type(obj);
        object_primitive_data* rhs_obj = get_object_true_type(rhs.val.carrier->obj);
        o->val = rhs_obj->val;
        env_push_object(&v->env, make_object_carrier(obj));
        vector_popn(v->env.bp, 2);
        LOG_DEBUG("\tinitvar\t\t%s\n", name);
        return (error_info){ .msg = NULL };
    }
    object* obj = make_object_primitive_data(name);
    object_primitive_data* o = get_object_true_type(obj);
    o->val = rhs;
    env_push_object(&v->env, make_object_carrier(obj));
    LOG_DEBUG("\tinitvar\t\t%s\n", name);
    vector_popn(v->env.bp, 2);
    return (error_info){ .msg = NULL };
}

INLINE static error_info assign(vm* v) {
    object_carrier* carrier = vector_backn(v->env.bp, 0).val.carrier;
    primitive_data* rhs = &vector_backn(v->env.bp, 1);
    if (carrier->obj->type == ObjectTypeFunctionDef) {
        vector_popn(v->env.bp, 2);
        return (error_info){ .msg = "function cannot be assigned by any kind!" };
    }
    if (carrier->obj->type == ObjectTypeRef) {
        ASSERT_MSG(0, "not implement ref object assignment");
    }

    if (rhs->type >= ObjectTypeNone) {
        if (rhs->type != ObjectTypePrimitiveData) {
            return (error_info){ .msg = "attmpt to add with a non primitive object" };
        }
        object_primitive_data* o = get_object_true_type(rhs->val.carrier->obj);
        *rhs = o->val;
    }

    object_primitive_data* o = get_object_true_type(carrier->obj);
    o->val = *rhs;
    LOG_DEBUG("\tassign\t\t%s\t", carrier->obj->name);
    print_primitive_data(&o->val);
    return (error_info){ .msg = NULL };
}

INLINE static error_info funcdef(vm* v) {
    cstring name = vector_backn(v->env.bp, 0).val.string;

    object_carrier* carrier = env_find_object(&v->env, name);
    if (carrier) {
        return (error_info){ .msg = "redefine name" };
    }

    i32 param_count = vector_backn(v->env.bp, 1).val.int32;
    vector_popn(v->env.bp, 2);
    object* obj = make_object_function_def(name);
    object_function_def* def = get_object_true_type(obj);
    def->entry_point = v->ip + 1;

    LOG_DEBUG("\tfuncdef\t\t%s %lld ", name, param_count);
    for (i32 i = 0; i < param_count; i++) {
        vector_push(def->args, vector_backn(v->env.bp, i).val.string);
        LOG_DEBUG_MSG("%s ", def->args[i]);
    }
    LOG_DEBUG_MSG("\n");
    vector_popn(v->env.bp, param_count);

    env_push_object(&v->env, make_object_carrier(obj));

    while (v->code[++v->ip] != ByteCodeFuncEnd);

    return (error_info){ .msg = NULL };
}

#define IMPL_ASSIGN(assign_name)\
        object_carrier* carrier = vector_backn(v->env.bp, 0).val.carrier;\
        primitive_data* rhs = &vector_backn(v->env.bp, 1);\
        if (rhs->type >= ObjectTypeNone) {\
            if (rhs->type != ObjectTypePrimitiveData) {\
                return (error_info){ .msg = "attmpt to add with a non primitive object" };\
            }\
            object_primitive_data* o = get_object_true_type(rhs->val.carrier->obj);\
            *rhs = o->val;\
        }\
        if (carrier->obj->type != ObjectTypePrimitiveData) {\
            vector_popn(v->env.bp, 2);\
            return (error_info){ .msg = "attemp to perform arithmetic operation on non primitive type" };\
        }\
        object_primitive_data* o = get_object_true_type(carrier->obj);\
        error_info ei = primitive_data_##assign_name##_assign(&o->val, rhs);\
        if (ei.msg) {\
            return ei;\
        }\
        LOG_DEBUG("\t" #assign_name "_assign\t%s\t", carrier->obj->name);\
        print_primitive_data(&o->val);\
        break;

#define IMPL_ARITHMETIC(name)\
        u32 end = vector_size(v->env.bp);\
        primitive_data data;\
        primitive_data* lhs = v->env.bp + end - 2;\
        primitive_data* rhs = v->env.bp + end - 1;\
        if (lhs->type >= ObjectTypeNone) {\
            if (lhs->type != ObjectTypePrimitiveData) {\
                return (error_info){ .msg = "attmpt to add with a primitive object" };\
            }\
            object_primitive_data* o = get_object_true_type(lhs->val.carrier->obj);\
            *lhs = o->val;\
        }\
        if (rhs->type >= ObjectTypeNone) {\
            if (rhs->type != ObjectTypePrimitiveData) {\
                return (error_info){ .msg = "attmpt to add with a primitive object" };\
            }\
            object_primitive_data* o = get_object_true_type(rhs->val.carrier->obj);\
            *rhs = o->val;\
        }\
        error_info ei = primitive_data_##name(&data, lhs, rhs);\
        if (ei.msg) {\
            return ei;\
        }\
        vector_popn(v->env.bp, 2);\
        vector_push(v->env.bp, data);\
        LOG_DEBUG("\t" #name "\t\t");\
        print_primitive_data(&data);

static error_info run(vm* v) {
    u32 size = vector_size(v->code);
    while(size > v->ip) {
        u8 code = v->code[v->ip];
        switch (code) {
        case ByteCodePushConst: {
            primitive_data data = { .type = v->code[v->ip+1] };
            memcpy(&data.val, &v->code[v->ip+2], primitive_size_map[data.type]);
            LOG_DEBUG("\tpush\t\t");
            print_primitive_data(&data);
            vector_push(v->env.bp, data);
            v->ip += primitive_size_map[data.type] + 1;
        } break;
        case ByteCodeAdd: { IMPL_ARITHMETIC(add) } break;
        case ByteCodeSub: { IMPL_ARITHMETIC(sub) } break;
        case ByteCodeMul: { IMPL_ARITHMETIC(mul) } break;
        case ByteCodeDiv: { IMPL_ARITHMETIC(div) } break;
        case ByteCodeMod: { IMPL_ARITHMETIC(mod) } break;
        case ByteCodeNegate: {
            primitive_data data;
            u32 end = vector_size(v->env.bp);
            error_info ei = primitive_data_neg(&data, v->env.bp + end - 1);
            if (ei.msg) {
                return ei;
            }
            vector_pop(v->env.bp);
            vector_push(v->env.bp, data);
            LOG_DEBUG("\tneg\t\t");
            print_primitive_data(&data);
        } break;
        case ByteCodeInitVar: {
            error_info ei = initvar(v);
            if (ei.msg) {
                return ei;
            }
        } break;
        case ByteCodePushName: {
            primitive_data data = {
                .val.string = (cstring)&v->code[v->ip+1],
                .type = PrimitiveDataTypeString,
            };
            vector_push(v->env.bp, data);
            LOG_DEBUG("\tpush\t\t%s\n", data.val.string);
            v->ip += 8;
            break;
        }
        case ByteCodeRefIden: {
            cstring name = (cstring)&v->code[v->ip+1];
            object_carrier* carrier = env_find_object(&v->env, name);
            if (!carrier) {
                return (error_info){ .msg = "not found object '%s'" };
            }
            primitive_data data = {
                .val.carrier = carrier,
                .type = carrier->obj->type,
            };
            vector_push(v->env.bp, data);
            LOG_DEBUG("\tref\t\t%s\n", name);
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
        case ByteCodeFuncDef: {
            error_info ei = funcdef(v);
            if (ei.msg) {
                return ei;
            }
            break;
        }
        case ByteCodeFuncEnd: {
            // i32 ret_ip = vector_
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

error_info vm_run(vm* v) {
    env_push_scope(&v->env);
    error_info ei = run(v);
    env_pop_scope(&v->env);
    return ei;
}

