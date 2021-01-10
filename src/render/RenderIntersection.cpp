#include "../util/Vector3.cpp"

struct RenderIntersection
{
    RenderIntersection(double distance, Vector3 color, bool collides=false) : distance(distance), color(color), collides(collides) {}
    RenderIntersection() : distance(-1), color(Vector3(0,0,0)) {}

    bool collides = false;
    double distance;
    Vector3 color;
};