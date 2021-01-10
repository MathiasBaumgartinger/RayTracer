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

class Node3d 
{
public:
    Node3d(std::string name, Vector3 position) : 
        name(name), position(position)  {}
    
    Node3d() : name("Node"), position(Vector3(0,0,0)) {}

    virtual void initFromXMLNode(pugi::xml_node node) {};

    virtual RenderIntersection intersectionTest(std::shared_ptr<RayCast> ray, Scene& scene, Camera& cam) 
    { std::cout << "in Node3d" << std::endl; return RenderIntersection(); }

    bool isVisible = false;
    Vector3 position;
    std::string name;
};