#pragma once

#include "Node3d.cpp"
#include "../Scene.cpp"
#include <math.h>

/*
* A raycast for raytracing, has a position and a castTo indicating the direction. Derived from Node3d.
*/
class RayCast : public Node3d 
{
public: 
    RayCast(std::string name, Vector3 position, Vector3 castTo) : Node3d(name, position), castTo(castTo) {}
    RayCast() : Node3d("Raycast", Vector3(0,0,0)), castTo(Vector3(0,0,-1)) {}

    /*
    * Trace an intersection point.
    */
    RenderIntersection trace(Scene& s, Camera& cam, int bounces=1)
    {
        double minDistance = DBL_MAX;
        RenderIntersection firstIntersection;
        for (auto obj : s.objects)
        {
            if(obj.get()->isVisible)
            {
                RenderIntersection intersection = obj->intersectionTest(std::make_shared<RayCast>(*this), s, cam, bounces);                
                
                if(intersection.collides && intersection.distance < minDistance)
                {
                    minDistance = intersection.distance;
                    firstIntersection = intersection;
                    setColliding(true);
                }
            }  
        }

        return firstIntersection;
    }

    bool isColliding() 
    {
        return collides;
    }

    void setColliding(bool isColliding) 
    {
        collides = isColliding;
    }
    
    Vector3 castTo;
private:
    bool collides = false;
};