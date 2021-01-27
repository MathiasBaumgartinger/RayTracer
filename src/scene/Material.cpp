#pragma once

#include "../util/Vector3.cpp"
#include "../util/Util.cpp"
#include "../pugixml/src/pugixml.cpp"
#include "../lodepng/lodepng.h"
#include "../lodepng/lodepng.cpp"
#include <string>

/*
* Material for surfaces. Two different types, solid or textured.
*/
class Material 
{
public:
    enum Mode { Solid, Textured };

    Material(int mode, Vector3 phong, float reflectance, float transmittance,
    float refraction, Vector3 color=Vector3(1, 1, 1), std::string texturePath="") 
        : mode(mode), phong(phong), reflectance(reflectance), transmittance(transmittance), 
          refraction(refraction), color(color), texturePath(texturePath)
    {
        setMode(mode);
    }

    Material() 
        : mode(Solid), phong(Vector3(0.2, 0.2, 0.2)), reflectance(0.5), transmittance(0.5), 
          refraction(0.5), color(Vector3(0.4, 0.4, 0.4)), texturePath("")
    {
        setMode(mode);
    }
    
    void setMode(int mode) 
    {
        this->mode = mode;
        if(mode == Solid)
        {
    
        } 
        else if(mode == Textured)
        {
            int error = lodepng::decode(texture, textureWidth, textureHeight, texturePath);
            if (error) setMode(Solid); 
        }
    }

    Vector3 getColorAt(double u, double v)
    {
        if(mode == Solid)
        {
            return color;
        } 
        else if(mode == Textured)
        {
            while (u > 1)
                u--;
            while (v > 1)
                v--;
                
            int ut = u * textureWidth;
            int vt = v * textureHeight;
            
            //std::cout <<  ut << ", " << vt << std::endl;
            int index = ut + textureWidth * vt * 4;
            index = (ut + textureWidth * vt) * 4;
            float r = static_cast<float>(texture[index]) / 255; 
            float g = static_cast<float>(texture[index+1]) / 255;
            float b = static_cast<float>(texture[index+2]) / 255;
            Vector3 texel = Vector3(r,g,b);
            return texel;
        }
        return Vector3(1,1,1);
    }

    /*
    * Initializer for the specified XML format, overriden from node3d
    */
    void initFromXMLNode(pugi::xml_node node, int mode) 
    {
        pugi::xml_node colorNode = node.child("color");
        pugi::xml_node phongNode = node.child("phong");
        pugi::xml_node reflectanceNode = node.child("reflectance");
        pugi::xml_node transmittanceNode = node.child("transmittance");
        pugi::xml_node refractionNode = node.child("refraction");
        pugi::xml_node textureNode = node.child("texture");
        if (!textureNode.empty())
        {
            texturePath = std::string("../scenes/") + textureNode.attribute("name").as_string();
            setMode(Textured);
        }

        color = Util::vec3FromXML(colorNode, "r", "g", "b");
        phong = Util::vec3FromXML(phongNode, "ka", "kd", "ks");
        phongExp = phongNode.attribute("exponent").as_float();
        reflectance = reflectanceNode.attribute("r").as_float();
        transmittance = transmittanceNode.attribute("t").as_float();
        refraction = refractionNode.attribute("iof").as_float();
    }

    int mode;
    Vector3 phong;
    float reflectance;
    float transmittance;
    float refraction;
    float phongExp = 100;

    std::string texturePath;
    std::vector<unsigned char> texture;
    unsigned textureWidth, textureHeight;

    Vector3 color;
};