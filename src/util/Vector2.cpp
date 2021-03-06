#pragma once

#include <exception>
#include <iostream>

/*
* Vector2 class for doing calculations.
*/
class Vector2 {
public:
    Vector2(double x, double y) : x(x), y(y) {}

    double x;
    double y;

    Vector2 Add(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    Vector2 operator+(const Vector2& other) const {
        return Add(other);
    }

    Vector2 Multiply(const Vector2& other) const {
        return Vector2(x * other.x, y * other.y);
    }

    Vector2 operator*(const Vector2& other) const {
        return Multiply(other);
    }

    bool operator==(const Vector2& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Vector2& other) const {
        return !(*this == other);
    }
};

std::ostream& operator<<(std::ostream& stream, const Vector2& other)
{
    stream << "(" << other.x << ", " << other.y << ")";
    return stream;
}