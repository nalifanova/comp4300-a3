#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "Entity.hpp"
#include "Vec2.hpp"

class Physics
{
public:
    static Vec2 overlap(const Vec2& aPos, const Vec2& bPos, const Vec2& aSize, const Vec2& bSize);

    /**
     * Overlap if d > 0, No overlap if d < 0
     *      d = [abs(x1-x2), abs(y1-y2)]
     *      oX = (w1/2) + (w2/2) - d.x
     *      oY = (h1/2) + (h2/2) - d.y
     *      overlap = [oX, oY]
     */
    static Vec2 getOverLap(std::shared_ptr<Entity>& a, std::shared_ptr<Entity>& b);

    /**
     * Previous overlap
     *                          -------------
     *       Previou overlap y  |           |  O.y > 0
     *      -------------  - - -|           |  Movement from left or right side
     *      |           |       |           |   if pos.x > prevPos.x - from left
     *      |           |- - -  ------------    if pos.x < prevPos.x - from right
     *      ------------  No previous x overlap
     */
    static Vec2 getPreviouslyOverlap(std::shared_ptr<Entity>& a, std::shared_ptr<Entity>& b);
};

#endif //PHYSICS_HPP
