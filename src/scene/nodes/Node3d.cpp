#pragma once

#include "../../util/Vector3.cpp"
#include "../../util/Vector2.cpp"
#include "../../util/Util.cpp"
#include "../../render/RenderIntersection.cpp"
#include "../../pugixml/src/pugixml.cpp"
#include <string>
#include <map>
#include <algorithm>
#include <memory>

class Scene;
class Camera;
class RayCast;

/*
* Anything in the scene should be a node. Provides certain interfacess for implementing functionality.
*/
class Node3d 
{
public:
    Node3d(std::string name, Vector3 position) : 
        name(name), position(position)  {}
    
    Node3d() : name("Node"), position(Vector3(0,0,0)) {}

    /*
    * Initializer for the specified XML format, overriden from node3d. Any derived class should implement this
    */
    virtual void initFromXMLNode(pugi::xml_node node) {};

    /*
    * Returns the closest intersection in the scene with the given ray. -1 if no collision could be found.
    * Any derived class that represents a surface should implement this.
    */
    virtual RenderIntersection intersectionTest(std::shared_ptr<RayCast> ray, Scene& scene, Camera& cam, bool findColor=true, int bounces=1) 
    { return RenderIntersection(); }

    bool isVisible = false;
    Vector3 position;
    std::string name;
};