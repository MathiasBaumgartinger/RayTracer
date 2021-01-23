#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include "nodes/Node3d.cpp"
#include "nodes/AmbientLight.cpp"
#include "Environment.cpp"


/*
* Class that contains all the information about the specified scene.
* Scene contains an environment, all lights and all objects as well as the camera. 
*/
class Scene
{
public:
    Scene(Environment& env) : env(env) {}
    Scene()
    {
        env = Environment();
    }

    /*
    * Easy command for showing all nodes in the scene. Mainly debugging purposes
    */
    void printScene() 
    {
        std::cout << "Lights: " << std::endl;
        for(auto obj : lights)
        {
            std::cout << obj->name << std::endl;
        }

        std::cout << "Objects: " << std::endl;
        for(auto obj : objects)
        {
            std::cout << obj->name << std::endl;
        }
    }
    
    std::shared_ptr<Camera> currentCam;
    std::string output;
    std::vector<std::shared_ptr<Node3d>> objects;
    std::vector<std::shared_ptr<Node3d>> lights;
    Environment env;
};