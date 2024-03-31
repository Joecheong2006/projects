#pragma once

#include "util.h"
#include "Render.h"

class ConstraintTypeIdGenerator {
private:
    static i32 current;

public:
    static inline i32 GenerateId() {
        return current++;
    }

    static inline i32 GetCurrent() {
        return current;
    }

};

inline i32 ConstraintTypeIdGenerator::current;

#define GENERATE_CONSTRAINT_IDENTIFIER(identifier)\
    static inline i32 GetTypeId() {\
        static i32 id = ConstraintTypeIdGenerator::GenerateId();\
        return id;\
    }\
    virtual inline i32 getTypeId() const override {\
        return GetTypeId();\
    }\
    static inline const char* GetTypeName() {\
        return #identifier;\
    }\
    virtual inline const char* getTypeName() const override {\
        return GetTypeName();\
    }

class Constraint : public Drawable {
public:
    Constraint() {}
    virtual ~Constraint() {}


    virtual inline i32 getTypeId() const { return -1; };
    virtual inline const char* getTypeName() const { return "None"; };
    virtual void solve(const f64& dt) = 0;

};

