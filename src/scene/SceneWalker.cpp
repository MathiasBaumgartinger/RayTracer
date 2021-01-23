#include "../pugixml/src/pugixml.cpp"
#include "../util/Util.cpp"
#include "SceneFactory.cpp"
#include <iostream>
#include <memory>
#include <stdlib.h>

/*
* Walks a specified xml file and creates its corresponding nodes. 
* Uses the pugi_xml library: https://pugixml.org/ for more information
*/
struct SceneWalker: pugi::xml_tree_walker
{
    SceneWalker(SceneFactory& fac, Scene &scene) : fac(fac), scene(scene) {}

    virtual bool for_each(pugi::xml_node& node)
    {
        if(fac.nodes.count(Util::snakeToCamel(node.name())))
        {
            std::cout << "Found useable node: " << Util::snakeToCamel(node.name()) << std::endl;

            auto current = fac.create(Util::snakeToCamel(node.name()));
            current->initFromXMLNode(node);
            
            if (std::string(node.name()) == "camera")
            {
                scene.currentCam = std::dynamic_pointer_cast<Camera>(current);
            }
            else if(std::string(node.name()).find("light") != std::string::npos) 
            {
                if (std::string(node.name()) == "ambient_light")
                {
                    scene.env.ambientLight = *(std::dynamic_pointer_cast<AmbientLight>(current).get());
                }
                else
                {
                    scene.lights.push_back(current);
                }
            }
            else 
            {
                scene.objects.push_back(current);
            }
        }
        else if(std::string(node.name()) == "background_color")
        {
            scene.env.backgroundColor = Util::vec3FromXML(node, "r", "g", "b");
        }

        return true; // continue traversal
    }

    SceneFactory& fac;
    Scene& scene;
};