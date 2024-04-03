#pragma once

#include "ObjectTypeIdGenerator.h"
#include "Drawable.h"

#define GENERATE_CONSTRAINT_IDENTIFIER()\
    static inline i32 GetTypeId() {\
        static const i32 id = ObjectTypeIdGenerator::GenerateId();\
        return id;\
    }\
    virtual inline i32 getTypeId() const override {\
        return GetTypeId();\
    }

class Constraint : public Drawable {
public:
    Constraint() = default;
    virtual ~Constraint() = default;

    virtual inline i32 getTypeId() const { return -1; };
    virtual void solve(const f64& dt) = 0;

};

