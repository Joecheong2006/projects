#pragma once
#include "Object.h"
#include "mfwlog.h"
#include <list>

class Tracer: public Object {
private:
    std::list<glm::vec2> positions_trace;

public:
    GENERATE_OBJECT_IDENTIFIER();

    Tracer(Object* target);
    inline virtual void update(const f64& dt) override { TOVOID(dt); };
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override;

    i32 maxSamples = 100;
    f32 maxScale, minScale;
    f32 dr = 0.6f;

    Object* target;
    
};

