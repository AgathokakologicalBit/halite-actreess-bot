#pragma once

#include <vector>
#include <algorithm>
#include <math.h>
#include <map>

#include "../hlt/hlt.hpp"
#include "path.h"
#include "node.h"

class Pathfinder
{
public:
    /**
     * Creates a list of all planets within a bounding box formed by two locations.
     * @param left_most_point   a point that *must* be on the left. (or else negatives :( )
     * @param right_most_point  a point that *must* be on the right. (or else negatives :( )
     * @return                  a list of all planets within bounding box
    */
    static std::vector<const hlt::Planet *> get_in_bounds(hlt::Location * left_most_point, hlt::Location * right_most_point, const hlt::Map &map)
    {
        std::vector<const hlt::Planet *> planets;
        const double min_x = left_most_point->pos_x;
        const double min_y = left_most_point->pos_y;
        const double max_x = right_most_point->pos_x;
        const double max_y = right_most_point->pos_y;
        for (auto& planet : map.planets) {
            if (planet.location.pos_x >= min_x && planet.location.pos_x <= max_x) {
                if (planet.location.pos_y >= min_y && planet.location.pos_y <= max_y) {
                    planets.push_back(&planet);
                }
            }
        }

        return planets;
    }

    /**
     * Creates a list of all planets intersecting a line from an origin to a goal
     * @param origin    the location to start the line at.
     * @param goal      the location to end the line at.
     * @return planets  the planets to check for intersection
    */
    static std::vector<hlt::Planet *> get_intersecting(hlt::Location * origin, hlt::Location * goal, std::vector<hlt::Planet *> planets)
    {
        std::vector<hlt::Planet *> intersecting_planets;
        for (auto& planet : planets) {
            double distance_from_line = point_distance_from_line(
                origin->pos_x,
                origin->pos_y,
                goal->pos_x,
                goal->pos_y,
                planet->location.pos_x,
                planet->location.pos_y
            );

            if (distance_from_line <= planet->radius) {
                intersecting_planets.push_back(planet);
            }
        }

        return intersecting_planets;
    }

    /**
     * Computes the distance a point is from a line
     * @param line_start_x  the x of the starting point of the line
     * @param line_start_y  the y of the starting point of the line
     * @param line_end_x    the x of the ending point of the line
     * @param line_end_y    the y of the ending point of the line
     * @param point_x       the x of the point to measure from
     * @param point_y       the y of the point to measure from
     * @return              the distance from the point to the closest point on the line
    */
    static double point_distance_from_line(
        const double line_start_x,
        const double line_start_y,
        const double line_end_x,
        const double line_end_y,
        const double point_x,
        const double point_y
    )
    {
        return std::abs(
            (line_end_y - line_start_y) * point_x +
            (line_start_x - line_end_x) * point_y +
            (line_start_y - line_end_y) * point_x +
            (line_start_x - line_end_x) * point_y
        ) / std::sqrt(
            std::pow((line_end_y - line_start_y), 2.0) +
            std::pow((line_start_x - line_end_x), 2.0)
        );
    }

    /**
     * Sorts a list of planets by their distance from an origin
     * @param list    list of planets to sort
     * @param origin  origin point to sort planets from
    */
    static void sort_by_distance(std::vector<hlt::Planet *> planets, hlt::Location * origin)
    {
        std::sort(planets.begin(), planets.end(), new distance_sorter(origin));
    }

    /**
     * Builds a graph of points for navigating around a provided list of obstacles from location origin to goal
     * @param origin   the starting location
     * @param goal     the ending location
     * @param planets  a list of planets to build points around
     * @return         a list of locations forming a graph around the obstacles with first point being origin and last being goal
    */
    static std::map<int, Node *> build_graph(hlt::Location * origin, hlt::Location * goal, std::vector<hlt::Planet *> planets)
    {

    }

    /**
     * Finds the optimal path along a graph of points where the first point is the origin and the last point is the goal
     * @param path  the list of locations forming the graph where the first is the origin location and the last is the goal
     * @return      a path object
    */
    static Path * find_optimal_path(std::vector<hlt::Location *> path)
    {

    }

private:
    /**
     * Object used by std::sort to sort planet's based on their distance from an origin point
    */
    struct distance_sorter {
        hlt::Location * origin;
        distance_sorter(hlt::Location * origin) {
            this->origin = origin;
        }

        /**
         * Function that computes if a planet is closer or farther to the origin than another (used by std::sort)
        */
        bool operator () (hlt::Planet * first, hlt::Planet * second) {
            return first->location.get_distance_to(*this->origin) < second->location.get_distance_to(*this->origin);
        }
    };
};
