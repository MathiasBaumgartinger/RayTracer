#include "nodes/Node3d.cpp"
#include "nodes/Camera.cpp"
#include "nodes/Sphere.cpp"
#include "nodes/AmbientLight.cpp"
#include "nodes/ParallelLight.cpp"
#include "nodes/PointLight.cpp"
#include "Scene.cpp"
#include <map>
#include <string>
#include <functional>

class SceneFactory {
public:
    std::shared_ptr<Node3d> create(const std::string &alias) const 
    {
        auto it = nodes.find(alias);
        if (it != nodes.end())
            return it->second();
        else
            return std::shared_ptr<Node3d>(nullptr);
    }

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
        register_type<AmbientLight>("AmbientLight");
        register_type<ParallelLight>("ParallelLight");
        register_type<PointLight>("PointLight");
    }

    std::map<std::string,  std::function<std::shared_ptr<Node3d>()>> nodes;
};