#pragma once

#include "AmbientLight.cpp"

/*
* PointLight having color, position and direction.
*/
class Light : public Node3d 
{
public:
    Light()
        : Node3d("Light", Vector3(0,0,0)), color(Vector3(1,1,1)) {}

    Light(std::string name, Vector3 position, Vector3 color) 
        : Node3d(name, position), color(color){}

    /*
    * Initializer for the specified XML format, overriden from node3d
    */
    virtual void initFromXMLNode(pugi::xml_node node) override = 0;

    Vector3 color;
};

