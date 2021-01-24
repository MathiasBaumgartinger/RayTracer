#pragma once

#include "Node3d.cpp"
#include "RayCast.cpp"
#include "ParallelLight.cpp"
#include "PointLight.cpp"
#include "../Material.cpp"
#include "../../util/Math.cpp"
#include "../../util/Render.cpp"
#include "../../util/ObjImporter.cpp"
#include <memory>
#include <string>

class ObjImporter;

class Mesh : public Node3d
{
public:
    Mesh(std::string name, Vector3 position, Material& material) 
        : Node3d(name, position), material(material) 
    { 
        isVisible = true; 
    }
    
    Mesh() 
        : Node3d("Mesh", Vector3(0,0,0)), material(Material()) 
    { 
        isVisible = true; 
    }
    
    /*
    * Initializer for the specified XML format, overriden from node3d
    */
    void initFromXMLNode(pugi::xml_node node) override 
    {
        path = std::string("../scenes/") + node.attribute("name").as_string();
        ObjImporter obj;
        obj.import(path);
        vBuffer = obj.vBufferOrdered;
        vtBuffer = obj.vtBufferOrdered;
        vnBuffer = obj.vnBufferOrdered;

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
    }

    /*
    * Returns the closest intersection in the scene with the given ray. -1 if no collision could be found
    */
    virtual RenderIntersection intersectionTest(std::shared_ptr<RayCast> ray, Scene& scene, Camera& cam, bool findColor=true, int bounces=1) override
    {
        double minDist = DBL_MAX;
        Vector3 hitspot(0,0,0);
        int triangleIndex = 0;

        // For each triangle ...
        for(int i = 0; i < vBuffer.size(); i+=3)
        {
            Vector3 v0 = vBuffer[i] + position;
            Vector3 v1 = vBuffer[i+1] + position;
            Vector3 v2 = vBuffer[i+2] + position;
            //std::cout << "Triangle " << i << " at positions " << v0 << ", " << v1 << ", " << v2 << "\n"; 

            // compute plane's normal
            Vector3 v0v1 = v1 - v0;
            Vector3 v0v2 = v2 - v0; 

            // no need to normalize
            Vector3 N = v0v1.cross(v0v2); // N 
            double area2 = N.len();

            // Step 1: finding P
    
            // check if ray and plane are parallel ?
            double NdotRayDirection = N.dot(ray->castTo); 
            if (fabs(NdotRayDirection) < 0.000001) // almost 0 
                continue; // they are parallel so they don't intersect ! 

            // compute d parameter using equation 2
            double d = N.dot(v0);

            // compute t (equation 3)
            double t = (N.dot(ray->position) + d) / NdotRayDirection; 
            // check if the triangle is in behind the ray
            if (t < 0) continue; // the triangle is behind

            // compute the intersection point using equation 1
            Vector3 P = ray->position + t * ray->castTo; 
        
            // Step 2: inside-outside test
            Vector3 C(0,0,0); // vector perpendicular to triangle's plane 
        
            // edge 0
            Vector3 edge0 = v1 - v0; 
            Vector3 vp0 = P - v0; 
            C = edge0.cross(vp0); 
            if (N.dot(C) < 0) continue; // P is on the right side 
        
            // edge 1
            Vector3 edge1 = v2 - v1; 
            Vector3 vp1 = P - v1; 
            C = edge1.cross(vp1); 
            if (N.dot(C) < 0)  continue; // P is on the right side 
        
            // edge 2
            Vector3 edge2 = v0 - v2; 
            Vector3 vp2 = P - v2; 
            C = edge2.cross(vp2); 
            if (N.dot(C) < 0) continue; // P is on the right side; 
        
            if (t < minDist && t >= 0) 
            {
                minDist = t;
                hitspot = P;
                triangleIndex = i;
            }
        }

        if (minDist != DBL_MAX)
        {
            if (findColor)
            {
                Vector3 normal = ((vnBuffer[triangleIndex] + vnBuffer[triangleIndex+1] + vnBuffer[triangleIndex+2]) / 3).normalized();
                return colorAtPoint(hitspot, normal, minDist, scene, cam);
            }
            return RenderIntersection(minDist, Vector3(0,0,0), true);
        }
        
        return RenderIntersection();
    }

    /*
    * Get the color at a specified point on the mesh.
    */
    RenderIntersection colorAtPoint(Vector3 hitspot1, Vector3 normal, double distance, Scene& scene, Camera& cam, bool reflect=true)
    {
        Vector3 color = material.color;
        Vector3 lightIntensity = scene.env.ambientLight.color * material.phong.x;
        Vector3 specularColor(0,0,0);
        Vector3 toViewDir = (cam.position - hitspot1).normalized();

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
            RayCast shadowRay("", hitspot1 + toLightDir * 0.001, toLightDir);
            double minDistance = DBL_MAX;
            for (auto object : scene.objects)
            {
                if (object.get() == this) continue;
                double intersectionDistance = object->intersectionTest(std::make_shared<RayCast>(shadowRay), scene, cam, false).distance;
                if (intersectionDistance >= 0 && intersectionDistance < minDistance)
                {
                    minDistance = intersectionDistance;
                }
            }
            if (minDistance < abs((light->position - hitspot1).len())) 
            {
                continue;
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

    std::vector<Vector3> vBuffer;
    std::vector<Vector3> vnBuffer;
    std::vector<Vector3> vtBuffer;

    std::string path;
    Material material;
};