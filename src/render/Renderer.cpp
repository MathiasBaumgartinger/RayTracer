#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <thread>
#include <cmath>
#include "../util/Math.cpp"
#include "../scene/nodes/Camera.cpp"
#include "../scene/nodes/RayCast.cpp"
#include "../util/ProgressBar.cpp"

class Renderer
{
public:
    Renderer(Camera &cam, Scene &s) : cam(cam), scene(s) {}

    /*
    *  Render the scene with given camera and scene
    */
    void render()
    {
        std::cout << "starting to write to: " << currentFileName << "\n\n";
        ProgressBar progress{std::clog, 70u, "Working"};
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
                writeToPPM(ray, maxColorValue);

            }
            progress.write(u / cam.resolution.x);
        }

        outfile.close();
    }
    std::string currentFileName = "test";
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
            normalized = normalized * ++maxColorValue; 

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
                // If the color codes excceed 255 because of high components, etc. take 255 instead
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

    std::ofstream outfile;
    Camera &cam;
    Scene &scene;
    bool debug = false;
};

