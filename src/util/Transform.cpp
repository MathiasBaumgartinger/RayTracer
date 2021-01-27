#pragma once

#include "Vector3.cpp"
#include "Vector4.cpp"
#include <iostream>

class Transform 
{
public: 
    // Default returns unity matrix
    Transform() 
    {
        matrix[0] = Vector4(1,0,0,0);
        matrix[1] = Vector4(0,1,0,0);
        matrix[2] = Vector4(0,0,1,0);
        matrix[3] = Vector4(0,0,0,1);
    }

    Transform(Vector4 vec0, Vector4 vec1, Vector4 vec2, Vector4 vec3)
    {
        matrix[0] = vec0;
        matrix[1] = vec1;
        matrix[2] = vec2;
        matrix[3] = vec3;
    }

    Vector3 position()
    {
        return Vector3(matrix[3][0], matrix[3][1], matrix[3][2]);
    }

    Vector4 Multiply(const Vector4& other) const 
    {
        return Vector4(
            (matrix[0] * other.x).sumComponents(), 
            (matrix[1] * other.y).sumComponents(),
            (matrix[2] * other.z).sumComponents(),
            (matrix[3] * other.w).sumComponents()
        );
    }

    Vector4 operator*(const Vector4& other) const
    {
        return Multiply(other);
    }

    Vector4 matrix[4];
};

namespace mat4 
{
    Transform lookAt(Vector3 eye, Vector3 at, Vector3 up)
    {
        Vector3 zaxis = (at - eye).normalized();
        Vector3 xaxis = (zaxis.cross(up)).normalized();
        Vector3 yaxis = xaxis.cross(yaxis);

        zaxis = -zaxis;

        Transform viewMatrix(
            Vector4(xaxis.x, yaxis.x, zaxis.z, eye.x),
            Vector4(xaxis.y, yaxis.y, zaxis.y, eye.y),
            Vector4(xaxis.z, yaxis.z, zaxis.z, eye.z),
            Vector4(0,0,0,1)
        );

        return viewMatrix;
    }
}

std::ostream& operator<<(std::ostream& stream, const Transform& other)
{
    stream << other.matrix[0] << std::endl;
    stream << other.matrix[1] << std::endl;
    stream << other.matrix[2] << std::endl;
    stream << other.matrix[3] << std::endl;
    return stream;
}
