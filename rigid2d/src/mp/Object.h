#pragma once
#include "ObjectTypeIdGenerator.h"
#include "Drawable.h"

#define GENERATE_OBJECT_IDENTIFIER()\
    inline static i32 GetTypeId() {\
        static const i32 id = ObjectTypeIdGenerator::GenerateId();\
        return id;\
    }\
    inline virtual i32 getTypeId() const override {\
        return GetTypeId();\
    }

class Object : public Drawable {
public:
    Object() = default;
    virtual ~Object() = default;
    virtual i32 getTypeId() const;

};

