#include "Physics.hpp"

Vec2 Physics::overlap(const Vec2& aPos, const Vec2& bPos, const Vec2& aSize, const Vec2& bSize)
{
    const Vec2 d = {abs(aPos.x - bPos.x), abs(aPos.y - bPos.y)};
    return {aSize.x / 2 + bSize.x / 2 - d.x, aSize.y / 2 + bSize.y / 2 - d.y};
}

Vec2 Physics::getOverLap(std::shared_ptr<Entity>& a, std::shared_ptr<Entity>& b)
{
    const auto& aPos = a->getComponent<CTransform>().pos;
    const auto& bPos = b->getComponent<CTransform>().pos;
    const auto& aSize = a->getComponent<CBoundingBox>().size; // width/height
    const auto& bSize = b->getComponent<CBoundingBox>().size;
    return overlap(aPos, bPos, aSize, bSize);
}

Vec2 Physics::getPreviouslyOverlap(std::shared_ptr<Entity>& a, std::shared_ptr<Entity>& b)
{
    const auto& aPos = a->getComponent<CTransform>().prevPos;
    const auto& bPos = b->getComponent<CTransform>().prevPos;
    const auto& aSize = a->getComponent<CBoundingBox>().size; // width/height
    const auto& bSize = b->getComponent<CBoundingBox>().size;
    return overlap(aPos, bPos, aSize, bSize);
}
