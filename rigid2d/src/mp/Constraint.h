#pragma once
#include "Object.h"

class Constraint : public Object {
public:
    inline virtual void update(const real& dt) { (void)dt; }

};

