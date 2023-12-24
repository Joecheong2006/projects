#ifndef _COLLISION_H
#define _COLLISION_H

namespace Collision {
    template <typename T>
    struct Colliable {
        virtual bool collide(T& obj) = 0;
        virtual void solve_collision(T& obj) = 0;
    };
}

#endif
