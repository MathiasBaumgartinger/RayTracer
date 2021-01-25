#pragma once

#include <exception>
#include <iostream>

/*
* Vector3 class for doing calculations.
*/
class Vector3 {
public:
    Vector3(double x, double y, double z) : x(x), y(y), z(z) {}

    Vector3(const Vector3& other)  : x(other.x), y(other.y), z(other.z) {}

    double x, y, z;

    double len() 
    {
        return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    }

    Vector3 normalized() 
    {
        return *this / len();
    }

    Vector3 Add(const Vector3& other) const 
    {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    Vector3 operator+(const Vector3& other) const 
    {
        return Add(other);
    }

    Vector3 Subtract(const Vector3& other) const 
    {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    Vector3 operator-(const Vector3& other) const 
    {
        return Subtract(other);
    }

    Vector3 operator-() const
    {
        return Vector3(-x, -y, -z);
    }

    Vector3 Multiply(const Vector3& other) const 
    {
        return Vector3(x * other.x, y * other.y, z * other.z);
    }

    Vector3 operator*(const Vector3& other) const 
    {
        return Multiply(other);
    }

    double dot(const Vector3& other) const 
    {
        return (x * other.x + y * other.y + z * other.z);
    }

    Vector3 cross(const Vector3& other) const 
    {
        //return Vector3(z * other.y - y * other.z, x * other.z - z * other.x, y * other.x - x * other.y);
        return Vector3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    }

    Vector3 Multiply(const float other) const 
    {
        return Vector3(x * other, y * other, z * other);
    }

    Vector3 operator*(const float other) const 
    {
        return Multiply(other);
    }

    Vector3 Divide(const double other) const 
    {
        return Vector3(x / other, y / other, z / other);
    }

    Vector3 operator/(const double other) const 
    {
        return Divide(other);
    }

    bool operator==(const Vector3& other) const 
    {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const Vector3& other) const 
    {
        return !(*this == other);
    }

    bool operator<(const Vector3& other) const 
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
        throw std::exception();
    }
};

std::ostream& operator<<(std::ostream& stream, const Vector3& other)
{
    stream << "(" << other.x << ", " << other.y << ", " << other.z << ")";
    return stream;
}

Vector3 operator*(double d, const Vector3& other)
{
    return other * d;
}