#pragma once

#include "AmbientLight.cpp"

/*
* PointLight having color, position and direction.
*/
class PointLight : public Node3d 
{
public:
    PointLight()
        : Node3d("ParallelLight", Vector3(0,0,0)), color(Vector3(1,1,1)), direction(Vector3(0.5, -0.5, 0)) {}

    PointLight(std::string name, Vector3 position, Vector3 color, Vector3 direction) 
        : Node3d(name, position), color(color), direction(direction) {}

    /*
    * Initializer for the specified XML format, overriden from node3d
    */
    void initFromXMLNode(pugi::xml_node node) override 
    {
        pugi::xml_node colorNode = node.child("color");
        color = Util::vec3FromXML(colorNode, "r", "g", "b");
        pugi::xml_node positionNode = node.child("position");
        position = Util::vec3FromXML(positionNode, "x", "y", "z");
    }

    Vector3 color;
    Vector3 direction;
};

