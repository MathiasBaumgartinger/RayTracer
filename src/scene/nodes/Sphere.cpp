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
    virtual RenderIntersection intersectionTest(std::shared_ptr<RayCast> ray, Scene& scene, Vector3 from, bool findColor=true, int bounces=1) override
    {
        Vector3 rayToCenter = position - ray->position;

        // Return no collision
        double rayToCenterNormal =  rayToCenter.dot((ray->castTo).normalized());
        if (rayToCenterNormal < 0) 
            return RenderIntersection(); // Default constructor says it does not collide

        // Return no collision
        double distanceToCenterNormal = sqrt(rayToCenter.dot(rayToCenter) - pow(rayToCenterNormal, 2));
        if (distanceToCenterNormal < 0 || distanceToCenterNormal > radius) 
            return RenderIntersection(); // Default constructor says it does not collide

        double difference = sqrt(pow(radius, 2) - pow(distanceToCenterNormal, 2));
        double distance1 = rayToCenterNormal - difference;
        Vector3 hitspot1 = ray->castTo.normalized() * distance1 + ray->position;

        // For finding the shadow
        if (!findColor) return RenderIntersection(distance1, Vector3(0,0,0), true);

        Vector3 d = (hitspot1 - position).normalized();
        double ut = 0.5 + atan2f(d.x, d.z) / 2*PI;
        double vt = 0.5 - asin(d.y) / PI;
        Vector3 color = material.getColorAt(ut, vt);

        return colorAtPoint(color, hitspot1, distance1, scene, from, bounces);
    }

    /*
    * Get the color at a specified point on the sphere.
    */
    RenderIntersection colorAtPoint(Vector3 color, Vector3 hitspot1, double distance, Scene& scene, Vector3 from, int bounces)
    {
        //std::cout << "Starting to find color ...\n";
        Vector3 normal = (hitspot1 - position).normalized();

        Vector3 lightIntensity = scene.env.ambientLight.color * material.phong.x;
        Vector3 specularColor(0,0,0);
        Vector3 reflectanceColor(0,0,0);
        Vector3 refractionColor(0,0,0);
        Vector3 toViewDir = (from - hitspot1).normalized();

        // Phong shading
        for (auto node : scene.lights)
        {
            std::shared_ptr<Light> light = std::dynamic_pointer_cast<Light>(node);
            std::shared_ptr<ParallelLight> parellelLight = std::dynamic_pointer_cast<ParallelLight>(node);
            Vector3 toLightDir(0,0,0);

            // Is parallel?
            if (parellelLight)
            {
                toLightDir = (parellelLight->direction * -1).normalized();
            }
            else
            {
                toLightDir = (light->position - hitspot1).normalized();
            }

            // Check for shadow
            RayCast shadowRay("shadow", hitspot1 + toLightDir * 0.001, toLightDir);
            double minDistance = DBL_MAX;
            for (auto object : scene.objects)
            {
                double intersectionDistance = object->intersectionTest(std::make_shared<RayCast>(shadowRay), scene, from, false).distance;
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

            Vector3 reflected = 2.0f * (normal.dot(toLightDir)) * normal - toLightDir;
            if(diffuse > 0.0) 
            {
                double angle = std::max((reflected.dot(toViewDir)), 0.0);
                specular = pow(angle, material.phongExp);
            }
            specularColor = specularColor + light->color * specular * material.phong.z;

            lightIntensity = lightIntensity + light->color * diffuse * material.phong.y;

            if((material. transmittance > 0 || material.reflectance > 0) && bounces > 0)
            {
                double cosTheta = -toViewDir.dot(normal);
                bool inside = cosTheta > 0;
                
                double n1;
                double n2;
                if (inside)
                {
                    n1 = material.refraction;
                    n2 = 1;

                    normal = -normal;
                }
                else 
                {
                    n2 = material.refraction;
                    n1 = 1;

                    cosTheta = -cosTheta;
                }
                double schlick = Util::schlickApprox(n1, n2, cosTheta);

                if  (!inside && material.reflectance > 0) 
                {
                    Vector3 reflectionDir = 2.0f * (normal.dot(toViewDir)) * normal - toViewDir;
                    RayCast reflectionRay("reflection", hitspot1, reflectionDir);

                    reflectanceColor = reflectanceColor +  trace(scene, reflectionRay, hitspot1, bounces, material.reflectance);
                }

                if (material.transmittance > 0)
                {
                    double r = n1 / n2;
                    double d = 1.0 - pow(r, 2) * (1 - cosTheta * cosTheta); 
                    
                    Vector3 refractionDir(0,0,0);
                    
                    if (d >= 0)
                        refractionDir = r * (normal * (-normal.dot(-toViewDir)) - toViewDir) - normal * sqrt(d);
                    else
                        refractionDir = 2.0f * (normal.dot(toViewDir)) * normal - toViewDir;
                    
                    RayCast refractionRay("refraction", -(hitspot1 + refractionDir * 0.0001), refractionDir);
                    refractionColor = trace(scene, refractionRay, -(hitspot1 + refractionDir * 0.0001), bounces, material.refraction) * material.transmittance;
                }
                color = color + refractionColor + reflectanceColor;
                color = color * (1 - schlick) + specularColor * schlick;
            }
            else
            {
                color = color + specularColor;
            }
        }
        
        color = color * lightIntensity; 
        return RenderIntersection(distance, color, true);
    }

    Vector3 trace(Scene& scene, RayCast ray, Vector3 from, int bounces, double multiplier)
    {
        Vector3 returnColor(0,0,0);
        for (auto object : scene.objects)
        {
            if (object.get() == this) continue;

            RenderIntersection traceIntersection = object->intersectionTest(std::make_shared<RayCast>(ray), scene, from, true, --bounces);
            if (traceIntersection.collides)
            {
                returnColor = returnColor + multiplier * traceIntersection.color;
            }
            else
            {
                returnColor = returnColor + multiplier * (scene.env.backgroundColor);
            }
        }

        return returnColor;
    }

    float radius;
    Material material;
};