#pragma once

#include "Node3d.cpp"
#include "../../util/Math.cpp"
#include "../../util/Transform.cpp"

/*
* Camera node with lookAt, up, field of view (x, y) and resolution. Derived from Node3d.
* maxBounce determines the amount of bouncing it should do while raytracing.
*/
class Camera : public Node3d 
{
public:
    Camera(std::string name, Vector3 position, Vector3 lookAt, Vector3 up, 
        float fov, Vector2 resolution, int maxBounces) : 
        Node3d(name, position), lookAt(lookAt), up(up), fov(fov),
        resolution(resolution), maxBounces(maxBounces)  
    {
        fovy = (resolution.y / resolution.x) * fov;
        lookAtTransform = mat4::lookAt(position, lookAt, up);
    }

    Camera() : Node3d("Camera", Vector3(0,0,0)), lookAt(Vector3(0, 0, -1)), up(Vector3(0, 1, 0)), 
        fov(45), resolution(Vector2(512, 512)), maxBounces(8)
    {
        fovy = (resolution.y / resolution.x) * fov;
    }

    /*
    * Initializer for the specified XML format, overriden from node3d
    */
    void initFromXMLNode(pugi::xml_node node) override 
    {
        pugi::xml_node posNode = node.child("position");
        pugi::xml_node lookAtNode = node.child("lookat");
        pugi::xml_node upNode = node.child("up");
        pugi::xml_node fovNode = node.child("horizontal_fov");
        pugi::xml_node resNode = node.child("resolution");
        pugi::xml_node maxBNode = node.child("max_bounces");

        position = Util::vec3FromXML(posNode);
        lookAt = Util::vec3FromXML(lookAtNode);
        up = Util::vec3FromXML(upNode);
        fov = fovNode.attribute("angle").as_float();
        resolution = Util::vec2FromXML(resNode, "horizontal", "vertical");
        maxBounces = maxBNode.attribute("n").as_int();

        lookAtTransform = mat4::lookAt(position, lookAt, up);
    }

    Vector3 lookAt;
    Vector3 up; 
    float fov;
    float fovy;
    Vector2 resolution;
    int maxBounces;
    Transform lookAtTransform;
};
