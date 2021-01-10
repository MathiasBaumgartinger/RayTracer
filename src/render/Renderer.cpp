#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include "../util/Math.cpp"
#include "../scene/nodes/Camera.cpp"
#include "../scene/nodes/RayCast.cpp"

class Renderer
{
public:
    Renderer(Camera &cam, Scene &s) : cam(cam), scene(s) {}

    /*
    *  Render the scene with given camera and scene
    */
    void render()
    {
        int maxColorValue = 255;
        preparePPM("P3", maxColorValue);

        double tanX = tan(Math::deg2rad(cam.fov));
        double tanY = tan(Math::deg2rad(cam.fovy));
        for (double u =  cam.resolution.x; u > 0; u--)  
        {
            outfile << "\n";
            double y = ((2 * (u - 0.5) - cam.resolution.x) / cam.resolution.x) * tanX;
            for (double v = 0; v < cam.resolution.y; v++)
            {
                double x = ((2 * (v - 0.5) - cam.resolution.y) / cam.resolution.y) * tanY;

                RayCast ray("", cam.position, Vector3(x, y, -1));
                Vector3 normalized = ray.castTo;
                //std::cout << "(" << normalized.x << ", " << normalized.y << ", " << normalized.z << ")\n";
                writeToPPM(ray, maxColorValue);
            }
        }

        outfile.close();
    }

private:
    /*
    * Write the meta data to file
    */
    void preparePPM(std::string magicIdentifier, int maxValue)
    {
        outfile.open(currentFileName + ".ppm", std::ofstream::out | std::ofstream::trunc);
        outfile.close();
        outfile.open(currentFileName + ".ppm");
        outfile << magicIdentifier << "\n"
                << cam.resolution.x << " " << cam.resolution.y << "\n"
                << maxValue;
    }

    /*
    * Write the current ray (x, y, z -> r, g, b) encoded from 0 to 255 to file
    */
    void writeToPPM(RayCast &ray, int maxColorValue)
    {
        if (debug)
        {
            Vector3 normalized = (ray.castTo - cam.position).normalized();
            //std::cout << "(" << normalized.x << ", " << normalized.y << ", " << normalized.z << ")\n";
            normalized = normalized * ++maxColorValue; /*
            normalized = normalized * static_cast<double>(maxColorValue) / 2; Vector3(normalized.x + 1, normalized.y + 1, normalized.z + 1);
            */
            //std::cout << "(" << normalized.x << ", " << normalized.y << ", " << normalized.z << ")\n\n";

            normalized.x = floor(normalized.x);
            normalized.y = floor(normalized.y), normalized.z = floor(normalized.z);
            ray.trace(scene, cam);
            if (ray.isColliding())
                outfile << 255 << " " << 255 << " " << 255 << "\t";
            else
                outfile << normalized.x << " " << normalized.y << " " << normalized.z << "\t";
        }
        else 
        {
            RenderIntersection intersection = ray.trace(scene, cam);
            if (ray.isColliding())
            {
                intersection.color = Vector3(
                    std::min(intersection.color.x * 255, 255.0), 
                    std::min(intersection.color.y * 255, 255.0), 
                    std::min(intersection.color.z * 255, 255.0)
                    );
                outfile << intersection.color.x << " " << intersection.color.y << " " << intersection.color.z << "\t";
            }
            else
            {
                outfile << scene.env.backgroundColor.x << " " << scene.env.backgroundColor.y << " " << scene.env.backgroundColor.z << "\t";
            }
        }
    }

    std::string currentFileName = "test";
    std::ofstream outfile;
    Camera &cam;
    Scene &scene;
    bool debug = false;
};