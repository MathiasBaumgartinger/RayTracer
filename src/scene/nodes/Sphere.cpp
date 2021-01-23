#pragma once

#include "Node3d.cpp"
#include "RayCast.cpp"
#include "ParallelLight.cpp"
#include "PointLight.cpp"
#include "../Material.cpp"
#include "../../util/Math.cpp"
#include "../../util/Render.cpp"
#include <memory>

/*
* Simple Sphere Surface, derived from Node3d.
*/
class Sphere : public Node3d 
{
public:
    Sphere(std::string name, Vector3 position, float radius, Material& material) 
        : Node3d(name, position), radius(radius), material(material) { isVisible = true; }
    
    Sphere() 
        : Node3d("Sphere", Vector3(0,0,0)), radius(1), material(Material()) { isVisible = true; }
    
    /*
    * Initializer for the specified XML format, overriden from node3d
    */
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

    /*
    * Returns the closest intersection in the scene with the given ray. -1 if no collision could be found
    */
    virtual double intersectionTest(std::shared_ptr<RayCast> ray) override
    {
        Vector3 rayToCenter = position - ray->position;

        double rayToCenterNormal =  rayToCenter.dot((ray->castTo).normalized());
        if (rayToCenterNormal < 0) 
            return -1;

        double distanceToCenterNormal = sqrt(rayToCenter.dot(rayToCenter) - pow(rayToCenterNormal, 2));
        if (distanceToCenterNormal < 0 || distanceToCenterNormal > radius) 
            return -1;

        double difference = sqrt(pow(radius, 2) - pow(distanceToCenterNormal, 2));
        double distance1 = rayToCenterNormal - difference;

        return distance1;
    }

    /*
    * Get the color at a specified point on the sphere.
    */
    RenderIntersection colorAtPoint(Vector3 hitspot1, double distance, Scene& scene, Camera& cam) override
    {
        Vector3 normal = (hitspot1 - position).normalized();

        Vector3 color = material.color;
        Vector3 lightIntensity = scene.env.ambientLight.color * material.phong.x;
        Vector3 specularColor(0,0,0);
        Vector3 toViewDir = (cam.position - hitspot1).normalized();

        // Phong shading
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

            // Check for shadow
            RayCast shadowRay("", hitspot1 + toLightDir * 0.001, toLightDir);
            double minDistance = DBL_MAX;
            for (auto object : scene.objects)
            {
                double intersectionDistance = object->intersectionTest(std::make_shared<RayCast>(shadowRay));
                if (intersectionDistance >= 0 && intersectionDistance < minDistance)
                {
                    minDistance = intersectionDistance;
                }
            }
            if (minDistance < abs((light->position - hitspot1).len())) 
            {
                return RenderIntersection(minDistance, color * lightIntensity, true);
            }

            double diffuse = std::max((normal.dot(toLightDir)), 0.0);
            double specular = 0.0;

            if(diffuse > 0.0) 
            {
                Vector3 reflected = 2.0f * (normal.dot(toLightDir)) * normal - toLightDir;
                double angle = std::max((reflected.dot(toViewDir)), 0.0);
                specular = pow(angle, material.phongExp);
            }
            specularColor = specularColor + light->color * specular * material.phong.z;

            lightIntensity = lightIntensity + light->color * diffuse * material.phong.y;
        }
        
        color = color * lightIntensity; 
        return RenderIntersection(distance, color + specularColor, true);
    }

    float radius;
    Material material;
};