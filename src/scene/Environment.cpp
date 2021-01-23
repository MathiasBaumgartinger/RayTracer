#include "../util/Vector3.cpp"
#include "nodes/AmbientLight.cpp"

/*
* An environment for the scene.
* Has a background color and contains the ambientLight (see nodes/AmbientLight.cpp)
*/
class Environment 
{
public:
    Environment(Vector3 backgroundColor, AmbientLight ambientLight) 
        : backgroundColor(backgroundColor), ambientLight(ambientLight) {}
    
    Environment() : backgroundColor(Vector3(0.3, 0.3, 0.3)) { ambientLight = AmbientLight("ambient", Vector3(0,0,0), Vector3(0.3, 0.3, 0.3)); }
    
    Vector3 backgroundColor;
    AmbientLight ambientLight;
};