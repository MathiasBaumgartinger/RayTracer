#pragma once

#include "Vector3.cpp"
#include "Vector4.cpp"
#include "../pugixml/src/pugixml.cpp"
#include "../util/Util.cpp"
#include "../util/Math.cpp"
#include <iostream>
#include <memory>

class Transform 
{
public:

    void initFromXMLNode(pugi::xml_node node, bool inverse=false)  
    {
        Vector3 translation = Util::vec3FromXML(node.child("translate"));
        Vector3 scale = Util::vec3FromXML(node.child("scale"));

        float xRot = node.child("rotateX").attribute("theta").as_float();
        float yRot = node.child("rotateY").attribute("theta").as_float();
        float zRot = node.child("rotateZ").attribute("theta").as_float();

        Vector3 rotations(xRot, yRot, zRot);

        if (inverse)
        {
            rotations = -rotations;
            translation = -translation;
            
            Transform xRotT(
                Vector4(cos(Math::deg2rad(rotations.x)), -sin(Math::deg2rad(rotations.x)), 0, 0),
                Vector4(sin(Math::deg2rad(rotations.x)), cos(Math::deg2rad(rotations.x)), 0, 0),
                Vector4(0, 0, 1, 0),
                Vector4(0, 0, 0, 1)
            );

            Transform yRotT(
                Vector4(1, 0, 0, 0),
                Vector4(0, cos(Math::deg2rad(rotations.y)), -sin(Math::deg2rad(rotations.y)), 0),
                Vector4(0, sin(Math::deg2rad(rotations.y)), cos(Math::deg2rad(rotations.y)), 0),
                Vector4(0, 0, 0, 1)
            );

            Transform zRotT(
                Vector4(cos(Math::deg2rad(rotations.z)), 0, sin(Math::deg2rad(rotations.z)), 0),
                Vector4(0, 1, 0, 0),
                Vector4(-sin(Math::deg2rad(rotations.z)), 0, cos(Math::deg2rad(rotations.z)), 0),
                Vector4(0, 0, 0, 1)
            );

            Transform translationT(
                Vector4(1, 0, 0, translation.x),
                Vector4(0, 1, 0, translation.y),
                Vector4(0, 0, 1, translation.z),
                Vector4(0, 0, 0, 1)
            );

            Transform scaleT(
                Vector4(1/scale.x, 0, 0, 0),
                Vector4(0, 1/scale.y, 0, 0),
                Vector4(0, 0, 1/scale.z, 0),
                Vector4(0, 0, 0, 1)
            );

            memcpy(matrix, (scaleT * translationT * zRotT * yRotT * xRotT).matrix, sizeof(matrix));
        }
        else
        {
            Transform xRotT(
                Vector4(cos(Math::deg2rad(rotations.x)), -sin(Math::deg2rad(rotations.x)), 0, 0),
                Vector4(sin(Math::deg2rad(rotations.x)), cos(Math::deg2rad(rotations.x)), 0, 0),
                Vector4(0, 0, 1, 0),
                Vector4(0, 0, 0, 1)
            );

            Transform yRotT(
                Vector4(1, 0, 0, 0),
                Vector4(0, cos(Math::deg2rad(rotations.y)), -sin(Math::deg2rad(rotations.y)), 0),
                Vector4(0, sin(Math::deg2rad(rotations.y)), cos(Math::deg2rad(rotations.y)), 0),
                Vector4(0, 0, 0, 1)
            );

            Transform zRotT(
                Vector4(cos(Math::deg2rad(rotations.z)), 0, sin(Math::deg2rad(rotations.z)), 0),
                Vector4(0, 1, 0, 0),
                Vector4(-sin(Math::deg2rad(rotations.z)), 0, cos(Math::deg2rad(rotations.z)), 0),
                Vector4(0, 0, 0, 1)
            );

            Transform translationT(
                Vector4(1, 0, 0, translation.x),
                Vector4(0, 1, 0, translation.y),
                Vector4(0, 0, 1, translation.z),
                Vector4(0, 0, 0, 1)
            );

            Transform scaleT(
                Vector4(scale.x, 0, 0, 0),
                Vector4(0, scale.y, 0, 0),
                Vector4(0, 0, scale.z, 0),
                Vector4(0, 0, 0, 1)
            );

            memcpy(matrix, (zRotT * yRotT * xRotT * translationT * scaleT).matrix, sizeof(matrix));
        }
    }

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
            (matrix[0].x * other.x + matrix[0].y * other.y + matrix[0].z * other.z + matrix[0].w * other.w), 
            (matrix[1].x * other.x + matrix[1].y * other.y + matrix[1].z * other.z + matrix[1].w * other.w),
            (matrix[2].x * other.x + matrix[2].y * other.y + matrix[2].z * other.z + matrix[2].w * other.w),
            (matrix[3].x * other.x + matrix[3].y * other.y + matrix[3].z * other.z + matrix[3].w * other.w)
        );
    }

    Vector4 operator*(const Vector4& other) const
    {
        return Multiply(other);
    }

    Transform Multiply(const Transform& other) const 
    {
        return Transform(
            Vector4(
                matrix[0][0] * other.matrix[0][0] + matrix[0][1] * other.matrix[1][0] + matrix[0][2] * other.matrix[2][0] + matrix[0][3] * other.matrix[3][0],
                matrix[0][0] * other.matrix[0][1] + matrix[0][1] * other.matrix[1][1] + matrix[0][2] * other.matrix[2][1] + matrix[0][3] * other.matrix[3][1],
                matrix[0][0] * other.matrix[0][2] + matrix[0][1] * other.matrix[1][2] + matrix[0][2] * other.matrix[2][2] + matrix[0][3] * other.matrix[3][2],
                matrix[0][0] * other.matrix[0][3] + matrix[0][1] * other.matrix[1][3] + matrix[0][2] * other.matrix[2][3] + matrix[0][3] * other.matrix[3][3]
            ), 
            Vector4(
                matrix[1][0] * other.matrix[0][0] + matrix[1][1] * other.matrix[1][0] + matrix[1][2] * other.matrix[2][0] + matrix[1][3] * other.matrix[3][0],
                matrix[1][0] * other.matrix[0][1] + matrix[1][1] * other.matrix[1][1] + matrix[1][2] * other.matrix[2][1] + matrix[1][3] * other.matrix[3][1],
                matrix[1][0] * other.matrix[0][2] + matrix[1][1] * other.matrix[1][2] + matrix[1][2] * other.matrix[2][2] + matrix[1][3] * other.matrix[3][2],
                matrix[1][0] * other.matrix[0][3] + matrix[1][1] * other.matrix[1][3] + matrix[1][2] * other.matrix[2][3] + matrix[1][3] * other.matrix[3][3]
            ), 
            Vector4(
                matrix[2][0] * other.matrix[0][0] + matrix[2][1] * other.matrix[1][0] + matrix[2][2] * other.matrix[2][0] + matrix[2][3] * other.matrix[3][0],
                matrix[2][0] * other.matrix[0][1] + matrix[2][1] * other.matrix[1][1] + matrix[2][2] * other.matrix[2][1] + matrix[2][3] * other.matrix[3][1],
                matrix[2][0] * other.matrix[0][2] + matrix[2][1] * other.matrix[1][2] + matrix[2][2] * other.matrix[2][2] + matrix[2][3] * other.matrix[3][2],
                matrix[2][0] * other.matrix[0][3] + matrix[2][1] * other.matrix[1][3] + matrix[2][2] * other.matrix[2][3] + matrix[2][3] * other.matrix[3][3]
            ), 
            Vector4(
                matrix[3][0] * other.matrix[0][0] + matrix[3][1] * other.matrix[1][0] + matrix[3][2] * other.matrix[2][0] + matrix[3][3] * other.matrix[3][0],
                matrix[3][0] * other.matrix[0][1] + matrix[3][1] * other.matrix[1][1] + matrix[3][2] * other.matrix[2][1] + matrix[3][3] * other.matrix[3][1],
                matrix[3][0] * other.matrix[0][2] + matrix[3][1] * other.matrix[1][2] + matrix[3][2] * other.matrix[2][2] + matrix[3][3] * other.matrix[3][2],
                matrix[3][0] * other.matrix[0][3] + matrix[3][1] * other.matrix[1][3] + matrix[3][2] * other.matrix[2][3] + matrix[3][3] * other.matrix[3][3]
            )
        );
    }

    Transform operator*(const Transform& other) const
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
        Vector3 yaxis = xaxis.cross(zaxis);

        zaxis = -zaxis;

        Transform viewMatrix(
            Vector4(xaxis.x, yaxis.x, zaxis.x, eye.x),
            Vector4(xaxis.y, yaxis.y, zaxis.y, eye.y),
            Vector4(xaxis.z, yaxis.z, zaxis.z, eye.z),
            Vector4(0,0,0,1)
        );

        return viewMatrix;
    }

    Transform fromRotation(Vector3 rotations) 
    {
        Transform xRot(
            Vector4(cos(Math::deg2rad(rotations.x)), -sin(Math::deg2rad(rotations.x)), 0, 0),
            Vector4(sin(Math::deg2rad(rotations.x)), cos(Math::deg2rad(rotations.x)), 0, 0),
            Vector4(0, 0, 1, 0),
            Vector4(0, 0, 0, 1)
        );

        Transform yRot(
            Vector4(1, 0, 0, 0),
            Vector4(0, cos(Math::deg2rad(rotations.y)), -sin(Math::deg2rad(rotations.y)), 0),
            Vector4(0, sin(Math::deg2rad(rotations.y)), cos(Math::deg2rad(rotations.y)), 0),
            Vector4(0, 0, 0, 1)
        );

        Transform zRot(
            Vector4(cos(Math::deg2rad(rotations.z)), 0, sin(Math::deg2rad(rotations.z)), 0),
            Vector4(0, 1, 0, 0),
            Vector4(-sin(Math::deg2rad(rotations.z)), 0, cos(Math::deg2rad(rotations.z)), 0),
            Vector4(0, 0, 0, 1)
        );

        return xRot * yRot * zRot;
    }

    Transform fromTranslation(Vector3 translation) 
    {
        return Transform(
            Vector4(1, 0, 0, translation.x),
            Vector4(0, 1, 0, translation.y),
            Vector4(0, 0, 1, translation.z),
            Vector4(0, 0, 0, 1)
        );
    }

    Transform fromScale(Vector3 scale) 
    {
        return Transform(
            Vector4(scale.x, 0, 0, 0),
            Vector4(0, scale.y, 0, 0),
            Vector4(0, 0, scale.z, 0),
            Vector4(0, 0, 0, 1)
        );
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
