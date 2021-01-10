#pragma once

#include "Node3d.cpp"
#include "../Scene.cpp"
#include <math.h>


class RayCast : public Node3d 
{
public: 
    RayCast(std::string name, Vector3 position, Vector3 castTo) : Node3d(name, position), castTo(castTo) {}
    RayCast() : Node3d("Raycast", Vector3(0,0,0)), castTo(Vector3(0,0,-1)) {}

    RenderIntersection trace(Scene& s, Camera& cam)
    {
        double minDistance = DBL_MAX;
        RenderIntersection firstIntersection;
        for (auto obj : s.objects)
        {
            if(obj.get()->isVisible)
            {
                RenderIntersection intersection = obj.get()->intersectionTest(std::make_shared<RayCast>(*this), s, cam);
                if(intersection.collides && intersection.distance < minDistance)
                {
                    //std::cout << "Ray " << this << " collides with " << obj->name << " at position " << obj->position << "\n"; 
                    //std::cout << intersection.color << std::endl;
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