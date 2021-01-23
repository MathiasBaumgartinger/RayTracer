#include "../util/Vector3.cpp"

/*
* Renderintersection class that can store all the interesting data of a ratrace intersection.
*/
struct RenderIntersection
{
    RenderIntersection(double distance, Vector3 color, bool collides=false) : distance(distance), color(color), collides(collides) {}
    RenderIntersection() : distance(-1), color(Vector3(0,0,0)) {}

    bool collides = false;
    double distance;
    Vector3 color;
};