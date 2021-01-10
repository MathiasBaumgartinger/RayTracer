#pragma once

#include "AmbientLight.cpp"

class ParallelLight : public Node3d 
{
public:
    ParallelLight()
        : Node3d("ParallelLight", Vector3(0,0,0)), color(Vector3(1,1,1)), direction(0.5, -0.5, 0) {}

    ParallelLight(std::string name, Vector3 position, Vector3 color, Vector3 direction) 
        : Node3d(name, position), color(color), direction(direction) {}

    void initFromXMLNode(pugi::xml_node node) override 
    {
        pugi::xml_node colorNode = node.child("color");
        color = Util::vec3FromXML(colorNode, "r", "g", "b");
        pugi::xml_node directionNode = node.child("direction");
        direction = Util::vec3FromXML(directionNode, "x", "y", "z");
    }

    Vector3 color;
    Vector3 direction;
};

