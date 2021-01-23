#pragma once

#include "Vector3.cpp"

/*
* Render helper namespace
*/
namespace Render 
{
    Vector3 reflect(Vector3 I, Vector3 N)
    {
        I = I.normalized();
        N = N.normalized();
        return 2.0f * (N.dot(I)) * N - I;
    }
}