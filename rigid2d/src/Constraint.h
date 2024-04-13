#pragma once

#include "Object.h"

class Constraint : public Object {
    friend class World;
    virtual void update(const f64& dt) { (void)dt; }

};

