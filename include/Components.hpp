#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <SFML/Graphics.hpp>

#include "Animation.hpp"
#include "Vec2.hpp"

class Component
{
public:
    bool has = false;
};

class CTransform: public Component
{
public:
    CTransform() = default;

    explicit CTransform(const Vec2& p):
        pos(p) {};

    CTransform(const Vec2& p, const Vec2& sp, const Vec2& sc, float a):
        pos(p), prevPos(p), velocity(sp), scale(sc), angle(a) {}

    Vec2 pos = {0.0, 0.0};
    Vec2 prevPos = {0.0, 0.0};
    Vec2 scale = {1.0, 1.0};
    Vec2 velocity = {0.0, 0.0};
    float angle = 0;
};

class CLifeSpan: public Component
{
public:
    CLifeSpan() = default;

    explicit CLifeSpan(const int duration, const int frame):
        lifespan(duration), frameCreated(frame) {}

    int lifespan = 0;
    int frameCreated = 0;
};

class CInput: public Component
{
public:
    CInput() = default;

    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool slit = false;
    bool shoot = false;
    bool canShoot = true;
    bool canJump = true;
};

class CBoundingBox: public Component
{
public:
    CBoundingBox() = default;

    explicit CBoundingBox(const Vec2& s):
        size(s), halfSize(s.x / 2, s.y / 2) {}

    Vec2 size;
    Vec2 halfSize;
};

class CAnimation: public Component
{
public:
    CAnimation() = default;

    explicit CAnimation(const Animation& animation, const bool r = false):
        animation(animation), repeat(r) {}

    Animation animation;
    bool repeat = false;
};

class CGravity: public Component
{
public:
    CGravity() = default;

    explicit CGravity(const float g):
        gravity(g) {};

    float gravity = 0;
};

class CState: public Component
{
public:
    CState() = default;

    explicit CState(std::string s):
        state(std::move(s)) {};

    std::string state = "jumping";
};

#endif //COMPONENTS_HPP
