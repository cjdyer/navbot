#ifndef PURE_PURSUIT_H
#define PURE_PURSUIT_H

#include <vector>
#include <iostream>
#include <math.h>
#include <numeric>
#include <cmath>

struct point
{
    double x, y;

    friend std::ostream &operator<<(std::ostream &os, const point &pos)
    {
        os << "x : " << pos.x << " y : " << pos.y;
        return os;
    }
};

/* Pure Pursuit is an algorithm designed to find a path with a smooth and continuous curvature that approaches a series of
input points. The waypoint path, or the one that connects consecutive input points with straight lines, has corners that
we want to get rid of in order to make our path as efficient and smooth as possible. We do this by following not the
path itself as we move, but rather the intersection between this path and a circle with radius r centered on our Robot's
current position. Intuitively, you can imagine that the Robot will already have started changing direction before it
meets a vertex in the waypoint path, as by that point the intersection-based path will have already redirected the Robot
to some point ahead on the next segment of the waypoint path. */

namespace purepursuit
{
    /**
     * @return: An integer value (1, -1 or 0) that represents the sign of the input x
     */
    inline int sign(double x);

    /**
     * @param p1: A vector of length two in the format {Y, X} that contains Robot::y and Robot::x
     * @param p2: A vector of length two in the format {Y, X} that contains the Y and X coordinate of the target point
     * @return: Provides the distance between the two points using the standard distance formula
     */
    double distance(point p1, point p2);

    /**
     * @desc: Calculates the heading required for our Robot to continuously face forward while moving from @param p1 to @param
     * p2. This is important because it allows our intakes to face any balls we might run into instead of knocking them aside
     * @param p1: A vector of length two in the format {Y, X} that contains Robot::y and Robot::x
     * @param p2: A vector of length two in the format {Y, X} that contains the Y and X coordinate of the target point
     * @return: Returns the slope between two points in degrees, which can be translated easily into Robot heading.
     */
    double get_degrees(point p1, point p2);

    /**
      * @desc: Calculates the next step of our pure pursuit mvoement given a segment of the waypoint path and our current
        position
      * @param start: A vector of length two in the format {Y, X} containing the first point of a two-point segment of the
        waypoint path
      * @param end: A vector of length two in the format {Y, X} containing the second point of a two-point segment of the
        waypoint path
      * @param cur: A vector of length two in the format {Y, X} that contains Robot::y and Robot::x
      * @param radius: A scalar representing how far our lookahead distance should be- in other words, how early we should start
        turning in anticipation of a change in direction
     * @return: The intersection between a circle centered on our Robot's current position with radius @param radius and the
        line formed between @param start and @param end
     */
    point get_intersection(point start, point end, point cur, double radius);
}

#endif