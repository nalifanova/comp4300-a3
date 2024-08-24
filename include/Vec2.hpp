#ifndef VEC2_H
#define VEC2_H

class Vec2
{
public:
    Vec2() = default;
    Vec2(float xIn, float yIn);

    bool operator ==(const Vec2& rhs) const;
    bool operator !=(const Vec2& rhs) const;

    Vec2 operator +(const Vec2& rhs) const;
    Vec2 operator -(const Vec2& rhs) const;
    Vec2 operator /(float val) const;
    Vec2 operator *(float val) const;

    bool operator +=(const Vec2& rhs);
    bool operator -=(const Vec2& rhs);
    bool operator *=(float val);
    bool operator /=(float val);

    [[nodiscard]] float dist(const Vec2& rhs) const;
    [[nodiscard]] float length() const;
    [[nodiscard]] Vec2 normalize();

    float x = 0.0f;
    float y = 0.0f;
};

#endif //VEC2_H
