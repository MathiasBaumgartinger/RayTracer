#include "render/Renderer.cpp"
#include "scene/SceneWalker.cpp"
#include "util/ObjImporter.cpp"
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
    ObjImporter importer;
    Mesh mesh;
    importer.import("../scenes/box.obj", mesh);

    std::cout << "Printing vbuffer of mesh:" << std::endl;
    for (Vector3 vertex : mesh.vBuffer) 
    {
        std::cout << vertex << std::endl;
    }
    

    /*SceneFactory fac;

    Scene scene;
    SceneWalker walker(fac, scene);

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(argv[1]);
    if (!result)
    {
        std::cerr << "Could not load file" << std::endl;
        return -1;
    }
    doc.traverse(walker);

    Renderer rend(*scene.currentCam.get(), scene);
    rend.currentFileName = doc.child("scene").attribute("output_file").as_string();

    rend.render();*/
}
