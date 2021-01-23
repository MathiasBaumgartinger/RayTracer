#pragma once

#include <math.h>
#include "Vector3.cpp"

/*
* Math helper namespace
*/
namespace Math 
{
    # define PI           3.14159265358979323846
    double deg2rad(double degrees) 
    {
        return degrees * 4.0 * atan(1.0) / 180.0;
    }

    double rad2deg(double radians)
    {
        return radians * 180.0 / 4.0 / atan(1.0);
    }
}