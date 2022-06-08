#include "pure_pursuit.h"

#define squared(n) pow(n, 2)

inline int purepursuit::sign(double x)
{
    return (-2 * (x < 0.0)) + (-1 * (x == 0.0)) + 1;
}

double purepursuit::get_degrees(point p1, point p2)
{
    return atan2(p2.y - p1.y, p1.x - p2.x) * 180 / M_PI;
}

double purepursuit::distance(point p1, point p2)
{
    return sqrt(squared(p1.x - p2.x) + squared(p1.y - p2.y));
}

point purepursuit::get_intersection(point start, point end, point cur, double radius)
{
    point p1 {start.x - cur.x, start.y - cur.y};
    point p2 {end.x - cur.x, end.y - cur.y};

    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    float d = squared(sqrt(squared(dx) + squared(dy)));
    float D = p1.x * p2.y - p2.x * p1.y;
    float discriminant = sqrt(abs(squared(radius) * d - squared(D)));

    float x1 = ( D * dy + sign(dy) * dx * discriminant) / d;
    float y1 = (-D * dx + abs(dy) * discriminant) / d;
    float x2 = ( D * dy - sign(dy) * dx * discriminant) / d;
    float y2 = (-D * dx - abs(dy) * discriminant) / d;

    point intersection1{x1, y1};
    point intersection2{x2, y2};

    float distance1 = distance(p2, intersection1);
    float distance2 = distance(p2, intersection2);

    point calc1{(x1 + cur.x), (y1 + cur.y)};
    point calc2{(x2 + cur.x), (y2 + cur.y)};

    if (distance1 < distance2)
        return calc1;
    if (distance1 > distance2)
        return calc2;
    return calc1;
}