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
public:
    Object() = default;
    virtual ~Object() = default;
    virtual void update(const f64& dt);
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer);
    virtual i32 getTypeId() const;

};

