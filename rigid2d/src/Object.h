#pragma once
#include "ObjectTypeIdGenerator.h"
#include "Drawable.h"

#define GENERATE_OBJECT_IDENTIFIER()\
    static inline i32 GetTypeId() {\
        static const i32 id = ObjectTypeIdGenerator::GenerateId();\
        return id;\
    }\
    virtual inline i32 getTypeId() const override {\
        return GetTypeId();\
    }

class Object : public Drawable {
private:
    virtual void update(const f64& dt);

public:
    Object() = default;
    virtual ~Object() = default;
    virtual i32 getTypeId() const;

};

