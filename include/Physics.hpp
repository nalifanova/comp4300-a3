#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "Entity.hpp"
#include "Vec2.hpp"

class Physics
{
    Vec2 getOverLap(std::shared_ptr<Entity>& a, std::shared_ptr<Entity>& b);
    Vec2 getPreviouslyOverlap(std::shared_ptr<Entity>& a, std::shared_ptr<Entity>& b);
};

#endif //PHYSICS_HPP
