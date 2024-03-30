#pragma once
#include "util.h"
#include "glm/glm.hpp"

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

#define GENERATE_CONSTRAINT_IDENTIFIER()\
    static inline i32 GetTypeId() {\
        static i32 id = ConstraintTypeIdGenerator::GenerateId();\
        return id;\
    }\
    virtual inline i32 getTypeId() const override {\
        return GetTypeId();\
    }\


namespace mfw {
    class Renderer;
};

class Constraint {
public:
    Constraint() {}
    virtual ~Constraint() {}

    virtual inline i32 getTypeId() const { return -1; };
    virtual void solve(f64 dt) = 0;
    virtual void render(const glm::mat4& proj, mfw::Renderer& renderer) = 0;

};

