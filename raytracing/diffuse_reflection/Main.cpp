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
    float norm();
    Point operator +(Point &p);
    Point operator -(Point &p);
    Point operator *(float c);
    void operator =(Point p);
};

using Vector = Point;

float Point::dot(Point &p)
{
    return x * p.x + y * p.y + z * p.z;
}

float Point::norm()
{
    return sqrt(this->dot(*this));
}

Point Point::operator +(Point &p)
{
    return Point {x + p.x, y + p.y, z + p.z};
}

Point Point::operator -(Point &p)
{
    return Point {x - p.x, y - p.y, z - p.z};
}

Point Point::operator *(float c)
{
    return Point{c * x, c * y, c * z};
}

void Point::operator =(Point p)
{
    x = p.x;
    y = p.y;
    z = p.z;
}

Point Point::CanvasToViewport()
{
    return Point {x * V_W/C_W, y * V_H/C_H, V_D};
}

enum
{
    LIGHT_AMBIENT,
    LIGHT_POINT,
    LIGHT_DIRECTIONAL
};

class Light
{
    public:
        int type;
        float intensity;
        Point position, direction;

        Light(int t, float i, Point p);
};

Light::Light(int t, float i, Point p = Point{0.0f, 0.0f, 0.0f})
{
    type = t;
    intensity = i;

    if(t == LIGHT_POINT)
        position = p;
    else if(t == LIGHT_DIRECTIONAL)
        direction = p;
}

float ComputeLighting(Point p, Vector normal, std::vector<Light> &light_sources)
{
    float i = 0.0f;
    for(int k = 0; k < light_sources.size(); ++k) {
        if(light_sources[k].type == LIGHT_AMBIENT)
            i += light_sources[k].intensity;
        else {
            Vector light;
            if(light_sources[k].type == LIGHT_POINT)
                light = light_sources[k].position - p;
            else
                light = light_sources[k].direction;

            float n_dot_l = light.dot(normal);
            if(n_dot_l > 0)
                i += light_sources[k].intensity * n_dot_l/(normal.norm() * light.norm());
        }
    }
    return i;
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

std::vector<float> TraceRay(Point &origin, Point &direction, float t_min, float t_max,
        std::vector<Sphere> &sphere_list, std::vector<Light> &light_sources)
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

    // Compute point of intersection, normal
    direction = direction * closest_t;
    Point p = origin + direction;
    Vector n = p - closest_sphere->center;
    float norm_n = n.norm();
    n.x /= norm_n;
    n.y /= norm_n;
    n.z /= norm_n;
    std::vector<float> final_color = closest_sphere->color;
    float intensity = ComputeLighting(p, n, light_sources);
    final_color[0] = round(final_color[0] * intensity);
    final_color[1] = round(final_color[1] * intensity);
    final_color[2] = round(final_color[2] * intensity);

    return final_color;
}

int main(int argc, char **argv)
{
    // Create the scene
    std::vector<Sphere> sphere_list;
    sphere_list.push_back(Sphere(Point(0, -1, 3), std::vector<float>{255, 0, 0}, 1));
    sphere_list.push_back(Sphere(Point(2, 0, 4), std::vector<float>{0, 0, 255}, 1));
    sphere_list.push_back(Sphere(Point(-2, 0, 4), std::vector<float>{0, 255, 0}, 1));
    sphere_list.push_back(Sphere(Point(0, -5001, 0), std::vector<float>{255, 255, 0}, 5000));

    // Light Sources
    std::vector<Light> light_sources;
    light_sources.push_back(Light(LIGHT_AMBIENT, 0.2f));
    light_sources.push_back(Light(LIGHT_POINT, 0.6f, Point{2, 1, 0}));
    light_sources.push_back(Light(LIGHT_DIRECTIONAL, 0.2f, Vector{1, 4, 4}));

    Point origin{O_X, O_Y, O_Z};

    for(int y = C_H/2 - 1; y >= -C_H/2; --y) {
        for(int x = -C_W/2; x <= C_W/2 - 1; ++x) {
            Point converted = Point{x, y, 0}.CanvasToViewport();
            std::vector<float> color = TraceRay(origin, converted, 1, inf, sphere_list, light_sources);

            std::cout << color[0] << " " << color[1] << " " << color[2] << std::endl;
        }
    }
}    
