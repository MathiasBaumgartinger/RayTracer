#pragma once

#include "Node3d.cpp"

class AmbientLight : public Node3d 
{
public:
    AmbientLight(std::string name, Vector3 position, Vector3 color)
        : Node3d(name, position), color(color) {}
    
    AmbientLight()
        : Node3d("AmbientLight", Vector3(0,0,0)), color(Vector3(1,1,1)) {}

    /*
    * Initializer for the specified XML format, overriden from node3d
    */
    virtual void initFromXMLNode(pugi::xml_node node) override 
    {
        pugi::xml_node colorNode = node.child("color");
        color = Util::vec3FromXML(colorNode, "r", "g", "b");
    }

    Vector3 color;
};