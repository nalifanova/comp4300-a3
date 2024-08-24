#include "Vec2.hpp"

#include <math.h>

Vec2::Vec2(const float xIn, const float yIn):
    x(xIn), y(yIn) {}

bool Vec2::operator==(const Vec2& rhs) const
{
    return x == rhs.x && y == rhs.y;
}

bool Vec2::operator!=(const Vec2& rhs) const
{
    return x != rhs.x || y != rhs.y;
}

Vec2 Vec2::operator+(const Vec2& rhs) const
{
    return {x + rhs.x, y + rhs.y};
}

Vec2 Vec2::operator-(const Vec2& rhs) const
{
    return {x - rhs.x, y - rhs.y};
}

Vec2 Vec2::operator/(float val) const
{
    return {x / val, y / val};
}

Vec2 Vec2::operator*(float val) const
{
    return {x * val, y * val};
}

bool Vec2::operator+=(const Vec2& rhs)
{
    x += rhs.x;
    y += rhs.y;
    return true; // why bool??
}

bool Vec2::operator-=(const Vec2& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    return true; // why bool??
}

bool Vec2::operator*=(float val)
{
    x *= val;
    y *= val;
    return true; // why bool??
}

bool Vec2::operator/=(float val)
{
    if (val == 0)
    {
        return false;
    }

    x /= val;
    y /= val;
    return true;
}

float Vec2::dist(const Vec2& rhs) const
{
    const float d_x = abs(x - rhs.x);
    const float d_y = abs(y - rhs.y);
    const auto distance = sqrtf(d_x * d_x + d_y * d_y);
    return distance;
}

/**
 * Vector length is a distance it travels from origin (0.0, 0.0) to
 * desired destination        ___________
 *                      L = \/ x^2 + y^2
 */
float Vec2::length() const
{
    return sqrt(x * x + y * y);
}

/**
 * Normalizing a vector maintains its angle but changes its length to 1
 * (unit vector)
 *      N = Vec2(V.x / L, V.y / L)
 */
Vec2 Vec2::normalize()
{
    *this = (length() == 0.0f) ? Vec2(0.0f, 0.0f) : *this / length();
    return *this;
}
