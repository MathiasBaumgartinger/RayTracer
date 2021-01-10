#pragma once

#include "Node3d.cpp"
#include "RayCast.cpp"
#include "ParallelLight.cpp"
#include "PointLight.cpp"
#include "../Material.cpp"
#include "../../util/Math.cpp"
#include "../../util/Render.cpp"

class Sphere : public Node3d 
{
public:
    Sphere(std::string name, Vector3 position, float radius, Material& material) 
        : Node3d(name, position), radius(radius), material(material) { isVisible = true; }
    
    Sphere() 
        : Node3d("Sphere", Vector3(0,0,0)), radius(1), material(Material()) { isVisible = true; }
    
    void initFromXMLNode(pugi::xml_node node) override 
    {
        pugi::xml_node posNode = node.child("position");
        pugi::xml_node materialNode;
        if(node.child("material_solid")) 
        {
            materialNode = node.child("material_solid");
            material.initFromXMLNode(materialNode, Material::Solid);
        }
        else if(node.child("material_textured"))
        {
            materialNode = node.child("material_textured");
            material.initFromXMLNode(materialNode, Material::Textured);
        }

        position = Util::vec3FromXML(posNode);
        radius = node.attribute("radius").as_float();
    }

    virtual RenderIntersection intersectionTest(std::shared_ptr<RayCast> ray, Scene& scene, Camera& cam) override
    {
        Vector3 rayToCenter = position - ray->position;

        double rayToCenterNormal =  rayToCenter.dot((ray->castTo).normalized());
        if (rayToCenterNormal < 0) 
            return RenderIntersection();

        double distanceToCenterNormal = sqrt(rayToCenter.dot(rayToCenter) - pow(rayToCenterNormal, 2));
        if (distanceToCenterNormal < 0 || distanceToCenterNormal > radius) 
            return RenderIntersection();

        double difference = sqrt(pow(radius, 2) - pow(distanceToCenterNormal, 2));
        double distance1 = rayToCenterNormal - difference;

        Vector3 hitspot1 = (ray->castTo).normalized() * distance1 + ray->position;
        Vector3 normal = (hitspot1 - position).normalized();

        Vector3 color = material.color;
        Vector3 lightIntensity = scene.env.ambientLight.color * material.phong.x;
        Vector3 specularColor(0,0,0);
        Vector3 toViewDir = (cam.position - hitspot1).normalized();
        for (auto node : scene.lights)
        {
            std::shared_ptr<ParallelLight> light = std::static_pointer_cast<ParallelLight>(node);
            Vector3 toLightDir(0,0,0);

            // Is parallel?
            if (light)
            {
                toLightDir = (light->direction * -1).normalized();
            }
            else
            {
                toLightDir = (light->position - hitspot1).normalized();
            }

            double diffuse = std::max((normal.dot(toLightDir)), 0.0);
            double specular = 0.0;

            if(diffuse > 0.0) 
            {
                Vector3 reflected = 2.0f * (normal.dot(toLightDir)) * normal - toLightDir;
                double angle = std::max((reflected.dot(toViewDir)), 0.0);
                //if(angle >= 1) std::cout << angle << std::endl; else angle = 1.1;
                specular = pow(angle, material.phongExp);
            }
            specularColor = specularColor + light->color * specular * material.phong.z;

            lightIntensity = lightIntensity + light->color * diffuse * material.phong.y;
        }
        
        color = color * lightIntensity; 

        return RenderIntersection(distance1, color + specularColor, true);
    }

    float radius;
    Material material;
};