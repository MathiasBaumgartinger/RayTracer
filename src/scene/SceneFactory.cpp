#include "nodes/Node3d.cpp"
#include "nodes/Camera.cpp"
#include "nodes/Sphere.cpp"
#include "nodes/Mesh.cpp"
#include "nodes/AmbientLight.cpp"
#include "nodes/ParallelLight.cpp"
#include "nodes/PointLight.cpp"
#include "Scene.cpp"
#include <map>
#include <string>
#include <functional>

/*
* In order to be able to dynamically create classes from the specified XML file, I created a factory that stores each 
* node. Giving a predefined alias, one can create an object by a stored prototype.
*/
class SceneFactory {
public:
    /*
    * Use this function to create a new node by its alias. If the node can be found it will be returned 
    * (copy or default constructed depending on specification).
    * If it cannot be found a nullptr will be returned.
    */
    std::shared_ptr<Node3d> create(const std::string &alias) const 
    {
        auto it = nodes.find(alias);
        if (it != nodes.end())
            return it->second();
        else
            return std::shared_ptr<Node3d>(nullptr);
    }

    /*
    * Register a new type in the factory.
    *  Use this if you want to create clones of the prototype
    *    factory.register_type("Node3d", node3d);
    * Use this syntax if you want to create default constructed objects
    *   factory.register_type<Node3d>("Node3d");
    */
    template <class T>
    void register_type(const std::string &alias, T prototype = T()) 
    {
        nodes.emplace(alias, 
        [prototype]()->std::shared_ptr<Node3d>{return std::make_shared<T>(prototype);});
    }

    SceneFactory() 
    {
        register_type<Camera>("Camera");
        register_type<Node3d>("Node3d");
        register_type<Sphere>("Sphere");
        register_type<Mesh>("Mesh");
        register_type<AmbientLight>("AmbientLight");
        register_type<ParallelLight>("ParallelLight");
        register_type<PointLight>("PointLight");
    }

    std::map<std::string,  std::function<std::shared_ptr<Node3d>()>> nodes;
};