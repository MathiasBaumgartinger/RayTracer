#include "render/Renderer.cpp"
#include "scene/SceneWalker.cpp"
#include <filesystem>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <typeinfo>
#include <memory>

int main(int argc, char *argv[])
{
    SceneFactory fac;

    Scene scene;
    SceneWalker walker(fac, scene);

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("../scenes/example2.xml");
    if (!result)
    {
        std::cerr << "Could not load file" << std::endl;
        return -1;
    }
    doc.traverse(walker);

    Camera cam("MyCam", Vector3(0, 0, 1), Vector3(0, 0, -1), Vector3(0, 1, 0), 45,
               Vector2(512, 512), 8);
    Scene s;
    Material mat;
    Sphere sp("sp", Vector3(3, 2, -3), 1, mat);
    s.objects.push_back(std::make_shared<Sphere>(sp));
    Sphere sp1("sp1", Vector3(0, 0, -10), 3, mat);
    s.objects.push_back(std::make_shared<Sphere>(sp1));

    Renderer rend(cam, scene);

    /*RayCast ray("", Vector3(0,0,0), Vector3(0,0,-1));
    ray.getCollider(s);

    std::cout << ray.isColliding() << std::endl;*/
    
    rend.render();
}
