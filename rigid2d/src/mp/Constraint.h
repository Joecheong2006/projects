#pragma once

#include "Object.h"

class Constraint : public Object {
    friend class World;
public:
    virtual void update(const real& dt) { (void)dt; }

};

