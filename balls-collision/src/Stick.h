#ifndef _STICK_H
#define _STICK_H

#include "Object.h"

class Stick : public Object2D {
    f32 length;
    glm::vec2 pos[2];

    void update(const f32& frame);

};

#endif
