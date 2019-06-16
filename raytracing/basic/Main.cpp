#include <iostream>
#include <vector>
#include <utility>
#include <cmath>
#include "Parameters.h"

constexpr float inf = std::numeric_limits<float>::infinity();
std::vector<float> background_color {BACKGROUND_R, BACKGROUND_G, BACKGROUND_B};

class Point
{
    public:
    float x, y, z;
    Point() {}
    Point(float x, float y, float z):x(x), y(y), z(z) {}

    Point CanvasToViewport();
    float dot(Point &p);
    Point operator +(Point &p);
    Point operator -(Point &p);
    void operator =(Point &p);
};

using Vector = Point;

float Point::dot(Point &p)
{
    return x * p.x + y * p.y + z * p.z;
}

Point Point::operator +(Point &p)
{
    return Point {x + p.x, y + p.y, z + p.z};
}

Point Point::operator -(Point &p)
{
    return Point {x - p.x, y - p.y, z - p.z};
}

void Point::operator =(Point &p)
{
    x = p.x;
    y = p.y;
    z = p.z;
}

Point Point::CanvasToViewport()
{
    return Point {x * V_W/C_W, y * V_H/C_H, V_D};
}

class Sphere
{
    public:
    Point center;
    std::vector<float> color;
    float radius;

    Sphere(Point c, std::vector<float> clr, float r):center(c), color(clr), radius(r) {}
    std::pair<float, float> IntersectRaySphere(Point &origin, Point &direction);
};

std::pair<float, float> Sphere::IntersectRaySphere(Point &origin, Point &direction)
{
    Vector oc = origin - center;
    float k1 = direction.dot(direction);
    float k2 = 2 * oc.dot(direction);
    float k3 = oc.dot(oc) - radius * radius;

    float discriminant = k2 * k2 - 4 * k1 * k3;
    if(discriminant < 0)
        return std::pair<float, float>(inf, inf);

    float t1 = (-k2 + sqrt(discriminant)) / (2 * k1);
    float t2 = (-k2 - sqrt(discriminant)) / (2 * k1);
    return std::pair<float, float>(t1, t2);
}

std::vector<float> &TraceRay(Point &origin, Point &direction, float t_min, float t_max, std::vector<Sphere> &sphere_list)
{
    float closest_t = inf;
    Sphere *closest_sphere = nullptr;

    for(int i = 0; i < sphere_list.size(); ++i) {
        std::pair<float, float> t_pair = sphere_list[i].IntersectRaySphere(origin, direction);
        float t1 = t_pair.first;
        float t2 = t_pair.second;

        if(t1 >= t_min && t1 <= t_max && t1 < closest_t) {
            closest_t = t1;
            closest_sphere = &sphere_list[i];
        }
        
        if(t2 >= t_min && t2 <= t_max && t2 < closest_t) {
            closest_t = t2;
            closest_sphere = &sphere_list[i];
        }
    }

    if(closest_sphere == nullptr)
        return background_color;

    return closest_sphere->color;
}

int main(int argc, char **argv)
{
    // Create the scene
    std::vector<Sphere> sphere_list;
    sphere_list.push_back(Sphere(Point(0, -1, 3), std::vector<float>{255, 0, 0}, 1));
    sphere_list.push_back(Sphere(Point(2, 0, 4), std::vector<float>{0, 0, 255}, 1));
    sphere_list.push_back(Sphere(Point(-2, 0, 4), std::vector<float>{0, 255, 0}, 1));

    Point origin{O_X, O_Y, O_Z};

    for(int y = C_H/2 - 1; y >= -C_H/2; --y) {
        for(int x = -C_W/2; x <= C_W/2 - 1; ++x) {
            Point converted = Point{x, y, 0}.CanvasToViewport();
            std::vector<float> color = TraceRay(origin, converted, 1, inf, sphere_list);

            std::cout << color[0] << " " << color[1] << " " << color[2] << std::endl;
        }
    }
}    
