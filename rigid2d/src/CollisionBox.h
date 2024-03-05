#ifndef _COLLISION_BOX_H
#define _COLLISION_BOX_H

#include "glm/glm.hpp"
#include <vector>

namespace Collision {
    class Box {
        friend class Object2D;
    public:
        glm::vec2 pos, wah;
        Box(): pos(0), wah(0) {}
        Box(const glm::vec2& pos, const glm::vec2& wah): pos(pos), wah(wah) {}
    };
};

#endif
