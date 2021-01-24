#pragma once

#include "Light.cpp"

/*
* ParallelLight having color and direction. Position is redundant for this node.
* Also known as DirectionalLight.
*/
class ParallelLight : public Light 
{
public:
    ParallelLight() : Light(), direction(0.5, -0.5, 0) {}

    ParallelLight(std::string name, Vector3 position, Vector3 color, Vector3 direction) 
        : Light(name, position, color), direction(direction) {}

    /*
    * Initializer for the specified XML format, overriden from node3d
    */
    void initFromXMLNode(pugi::xml_node node) override 
    {
        pugi::xml_node colorNode = node.child("color");
        color = Util::vec3FromXML(colorNode, "r", "g", "b");
        pugi::xml_node directionNode = node.child("direction");
        direction = Util::vec3FromXML(directionNode, "x", "y", "z");
    }

    Vector3 direction;
};

