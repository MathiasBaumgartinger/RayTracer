#pragma once

#include <exception>
#include <iostream>
#include "Vector3.cpp"

/*
* Vector4 class for doing calculations.
*/
class Vector4 {
public:
    Vector4() : x(0), y(0), z(0), w(0) {}
    Vector4(Vector3 other) : x(other.x), y(other.y), z(other.z), w(1) {}
    Vector4(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {}

    Vector4(const Vector4& other)  : x(other.x), y(other.y), z(other.z), w(other.w) {}

    double x, y, z, w;

    Vector3 xyz()
    {
        return Vector3(x,y,z);
    }

    double len() 
    {
        return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2) + pow(z, 4));
    }

    Vector4 normalized() 
    {
        return *this / len();
    }

    Vector4 Add(const Vector4& other) const 
    {
        return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    Vector4 operator+(const Vector4& other) const 
    {
        return Add(other);
    }

    Vector4 Subtract(const Vector4& other) const 
    {
        return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    Vector4 operator-(const Vector4& other) const 
    {
        return Subtract(other);
    }

    Vector4 operator-() const
    {
        return Vector4(-x, -y, -z, -w);
    }

    Vector4 Multiply(const Vector4& other) const 
    {
        return Vector4(x * other.x, y * other.y, z * other.z, w * other.w);
    }

    Vector4 operator*(const Vector4& other) const 
    {
        return Multiply(other);
    }

    Vector4 Multiply(const float other) const 
    {
        return Vector4(x * other, y * other, z * other, w * other);
    }

    Vector4 operator*(const float other) const 
    {
        return Multiply(other);
    }

    Vector4 Divide(const double other) const 
    {
        return Vector4(x / other, y / other, z / other, w / other);
    }

    Vector4 operator/(const double other) const 
    {
        return Divide(other);
    }

    bool operator==(const Vector4& other) const 
    {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }

    bool operator!=(const Vector4& other) const 
    {
        return !(*this == other);
    }

    bool operator<(const Vector4& other) const 
    {
        return x < other.x || x == other.x && (y < other.y || y == other.y && z < other.z);
    }

    double operator[](const int index) const
    {
        if (index == 0) 
        {
            return x;
        }
        else if (index == 1)
        {
            return y;
        }
        else if (index == 2) 
        {
            return z;
        }
        else if (index == 3) 
        {
            return w;
        }
        return 0;
    }

    double & operator[](const int index)
    {
        if (index == 0) 
        {
            return x;
        }
        else if (index == 1)
        {
            return y;
        }
        else if (index == 2) 
        {
            return z;
        }
        else if (index == 3) 
        {
            return w;
        }
        throw std::exception();
    }

    double sumComponents()
    {
        return x + y + z + w;
    }
};

std::ostream& operator<<(std::ostream& stream, const Vector4& other)
{
    stream << "(" << other.x << ", " << other.y << ", " << other.z << ", " << other.w << ")";
    return stream;
}

Vector4 operator*(double d, const Vector4& other)
{
    return other * d;
}