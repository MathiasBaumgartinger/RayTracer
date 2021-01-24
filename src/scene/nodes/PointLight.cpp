#pragma once

#include "Light.cpp"

/*
* PointLight having color, position and direction.
*/
class PointLight : public Light 
{
public:
    PointLight() : Light() {}

    PointLight(std::string name, Vector3 position, Vector3 color) : Light(name, position, color){}

    /*
    * Initializer for the specified XML format, overriden from node3d
    */
    virtual void initFromXMLNode(pugi::xml_node node) override 
    {
        pugi::xml_node colorNode = node.child("color");
        color = Util::vec3FromXML(colorNode, "r", "g", "b");
        pugi::xml_node positionNode = node.child("position");
        position = Util::vec3FromXML(positionNode, "x", "y", "z");
    }

};

