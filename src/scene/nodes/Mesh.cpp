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
    virtual RenderIntersection intersectionTest(std::shared_ptr<RayCast> ray, Scene& scene, Vector3 from, bool findColor=true, int bounces=1) override
    {
        double minDist = DBL_MAX;
        Vector3 hitspot(0,0,0);
        int triangleIndex = 0;
        double ux,vy;

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
            
            //WIKIPEDIA
            Vector3 h = ray->castTo.cross(v0v2);
            double a = v0v1.dot(h);
            if (fabs(a) < 0.000001)
                continue;

            double f = 1.0/a;
            Vector3 s = ray->position - v0;
            double u = f * s.dot(h);
            if(u < 0.0 ||u > 1.0)
                continue;

            Vector3 q = s.cross(v0v1);
            double v = f * ray->castTo.dot(q);
            if(v < 0.0 || u + v > 1.0)
                continue;

            double t = f * v0v2.dot(q);

            if(t < minDist && t > 0.000001) 
            {
                minDist = t;
                hitspot = ray->position + t * ray->castTo;
                triangleIndex = i;

                //Vector3 N = v0v1.cross(v0v2);
                //float denom = N.dot(N);
                ux = u;// / denom;
                vy = v;// / denom;
            }

            //SCRATCHFROMPIXEL
            /*Vector3 N = v0v1.cross(v0v2); // N 
            double denom = N.dot(N);

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
            }*/
        }

        if (minDist != DBL_MAX)
        {
            if (findColor)
            {
                double ut = (1-ux-vy) * vtBuffer[triangleIndex].x + ux * vtBuffer[triangleIndex+1].x + vy * vtBuffer[triangleIndex+2].x;
                double vt = (1-ux-vy) * vtBuffer[triangleIndex].y + ux * vtBuffer[triangleIndex+1].y + vy * vtBuffer[triangleIndex+2].y;
                Vector3 color = material.getColorAt(ut, vt);
                Vector3 normal = ((vnBuffer[triangleIndex] + vnBuffer[triangleIndex+1] + vnBuffer[triangleIndex+2]) / 3).normalized();
                return colorAtPoint(color, hitspot, normal, minDist, scene, from, bounces);
            }
            return RenderIntersection(minDist, Vector3(0,0,0), true);
        }
        
        return RenderIntersection();
    }

    /*
    * Get the color at a specified point on the mesh.
    */
    RenderIntersection colorAtPoint(Vector3 color, Vector3 hitspot1, Vector3 normal, double distance, Scene& scene, Vector3 from, int bounces)
    {
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
                if (object.get() == this) continue;
                double intersectionDistance = object->intersectionTest(std::make_shared<RayCast>(shadowRay), scene, from, false).distance;
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
                    refractionColor = trace(scene, refractionRay, -(hitspot1 + refractionDir * 0.0001), bounces, material.refraction);
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
                returnColor = returnColor + multiplier * (scene.env.backgroundColor) - Vector3(0.05,0.05,0.05);
            }
        }

        return returnColor;
    }

    std::vector<Vector3> vBuffer;
    std::vector<Vector3> vnBuffer;
    std::vector<Vector3> vtBuffer;

    std::string path;
    Material material;
};