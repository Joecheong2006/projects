#include "vm.h"
#include "object.h"
#include "tracing.h"
#include "bytecode.h"
#include "core/log.h"
#include "core/assert.h"

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

static error_info initvar(vm* v) {
    cstring name = vector_backn(v->env.bp, 0).val.string;
    primitive_data rhs = vector_backn(v->env.bp, 1);
    if (rhs.type >= 0xf) {
        ASSERT_MSG(rhs.val.carrier->obj->type == ObjectTypePrimitiveData, "not implement initvar obj yet");
        object* obj = make_object_primitive_data(name);
        object_primitive_data* o = get_object_true_type(obj);
        object_primitive_data* rhs_obj = get_object_true_type(rhs.val.carrier->obj);
        o->val = rhs_obj->val;
        env_push_object(&v->env, make_object_carrier(obj));
        vector_popn(v->env.bp, 2);
        LOG_DEBUG("\tinitvar %s -> %d\n", name, o->val.val.int64);
        return (error_info){ .msg = NULL };
    }
    object* obj = make_object_primitive_data(name);
    object_primitive_data* o = get_object_true_type(obj);
    o->val = rhs;
    env_push_object(&v->env, make_object_carrier(obj));
    LOG_DEBUG("\tinitvar %s -> %d\n", name, o->val.val.int64);
    vector_popn(v->env.bp, 2);
    return (error_info){ .msg = NULL };
}

#define IMPL_ARITHMETIC(name)\
        u32 end = vector_size(v->env.bp);\
        primitive_data data;\
        primitive_data* lhs = v->env.bp + end - 2;\
        primitive_data* rhs = v->env.bp + end - 1;\
        if (lhs->type >= 0xf) {\
            if (lhs->type != ObjectTypePrimitiveData) {\
                return (error_info){ .msg = "attmpt to add with a primitive object" };\
            }\
            object_primitive_data* o = get_object_true_type(lhs->val.carrier->obj);\
            *lhs = o->val;\
        }\
        if (rhs->type >= 0xf) {\
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
        LOG_DEBUG("\t" #name " -> %d:%d\n", data.val.int64, data.type);

static error_info run(vm* v) {
    u32 size = vector_size(v->code);
    while(size > v->ip) {
        u8 code = v->code[v->ip];
        switch (code) {
        case ByteCodePushConst: {
            primitive_data data = {
                .val = { v->code[v->ip+2] },
                .type = v->code[v->ip+1]
            };
            LOG_DEBUG("\tpush %d:%d\n", data.val.int64, data.type);
            vector_push(v->env.bp, data);
            v->ip += 9;
        } break;
        case ByteCodeAdd: { IMPL_ARITHMETIC(add) } break;
        case ByteCodeSub: { IMPL_ARITHMETIC(minus) } break;
        case ByteCodeMul: { IMPL_ARITHMETIC(multiply) } break;
        case ByteCodeDiv: { IMPL_ARITHMETIC(divide) } break;
        case ByteCodeMod: { IMPL_ARITHMETIC(modulus) } break;
        case ByteCodeNegate: {
            primitive_data data;
            u32 end = vector_size(v->env.bp);
            error_info ei = primitive_data_negate(&data, v->env.bp + end - 1);
            if (ei.msg) {
                return ei;
            }
            vector_pop(v->env.bp);
            vector_push(v->env.bp, data);
            LOG_DEBUG("\tneg -> %d:%d\n", data.val.int64, data.type);
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
            v->ip += 8;
            break;
        }
        default: {
            LOG_ERROR("invalid bytecode %d\n", code);
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

